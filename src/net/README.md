## 1 全部类生命周期关系图

```
tree -I "build*|test*" -P "*.cpp|*.h"
```

外部类有  
EventLoop,Timer,Client,ClientPool,Server,TcpConnection
内部类有  
外部类的内部实现Connector,Acceptor,Asyncer  
TCP的封装Buffer,Socket,Event  
核心调度器实现AsyncPoller,Epoller,TimerPoller  

```mermaid
graph LR;
note[虚线是关联关系<br>实线是组合关系]
subgraph EventLoop
EpollEvent-.->EventLoop
EpollPoller-->EventLoop
TimerEvent-.->EventLoop
TimerPoller-->EventLoop
AsyncerEvent-.->EventLoop
AsyncerPoller-->EventLoop
end
Socket-->TcpConnection
Buffer-->TcpConnection
EpollEvent1[EpollEvent]-->TcpConnection
Socket1[Socket]-->Acceptor
EpollEvent2[EpollEvent]-->Acceptor
Socket2[Socket]-->Connector
EpollEvent3[EpollEvent]-->Connector
Acceptor-->Server
TcpConnection-->TcpConnectionSet
TcpConnectionSet-->Server
TcpConnection-->Client
Connector-->Client
TimerEvent1[TimerEvent]-->Timer
AsyncerEvent1[AsyncerEvent]-->Asyncer
```

## 2 内部类和函数

```
* 1 Connector
Connect
无法重复使用，因为一旦连接上了，内部成员会转移给TcpConenction
SetNewConnectionHandler
SetConnectFailedHandler
* 2 Acceptor
* 3 Asyncer
AsyncDo
Cancel
~AsyncDo
1 内部类，无法重复使用，重复使用会直接调用回调传递错误
2 ~AsyncDo和Cancel等效，如果被释放前没有执行回调会调用一次回调
* 4 Buffer
* 5 Socket
* 6 Event
* 7 AsyncerPoller
存储结构是list<event>
辅助存储结构hash<id, list::iter>
update先判断在hash中是否存在
不存在就是add，直接pushback，并且记录下iter用于后面可以删除，时间复杂度O(1)
存在就先remove在add，O(1)
remove是通过hash中拿到iter去list删除，O(1)
* 8 EpollerPoller
以fd作为标识符，设置读写事件
存储结构是vector<event>
events_作为数组存在，然后判断events_[fd]的状态来判断fd是否在epoll内核中
update先判断events_[fd]是否存在
不存在就add，直接赋值，并且epoll_add 用户操作O(1)，内核操作O(nlgn)
否则就直接epoll_ctl 没有用户操作，内核操作O(nlgn)
remove是赋值一个空的weak_ptr，用户操作O(1)，内核操作O(nlgn)
* 9 TimerPoller(TODO，性能可以优化到O(1))
存储结构是map<timeFd, map<id, event>>，主键是毫秒为单位的时间，值是一个map
辅助结构set<id>判断这个事件是否存在
update先判断set中是否存在
不存在就是add，直接insert到map，当同一毫秒有多个event时，会通过id的先后次序保证event的顺序性O(nlgn)
存在就先remove再add,O(nlgn)
remove是删除map的值再删除set的值O(nlgn)
```

PS:
2.1 目前只驱动了epoll的LT模式  
2.2 TcpConnection的readEvent是一直监视的,用来读取对端关闭连接，
writeEvent是只有建立了写事件才会注册，一旦完成了这一次写事件要写完的buffer就会取消注册  
2.3 stale event  
见man epoll的Possible pitfalls and ways to avoid them部分  
man的建议是通过一个clean list来判断已经被关闭的描述符  
我的实现是当一个TcpConnection被关闭的时候，他的event会被析构，从而使得执行的weak指针已经为空
以避免stale event。这种实现是通过C++的生命周期实现来考虑的。
给回调函数生命周期，当描述符被删除时（TcpConnection被销毁），回调函数也被销毁，保证不会再被调用。
2.4 线程安全性
内部类以及回调全部运行在EventLoop的循环中，因此所有内部类都没有被设计为线程安全的  
外部类在使用内部类时，使用Asyncer来把内容放入循环中(Asyncer非线程安全，需要加锁保护)

## 3 用户可调用的类和函数

```
* 1 EventLoop
Wait
Stop
* 2 Timer
AsyncWait
Cancel
* 3 Client
SetConnectedHandler
SetDisconnectedHandler
AsyncConnect
GetTcpConnection
Reset
如果报错的话直接调用TcpConnection的Finish，否则Reset TcpConnection进行复用
* 4 ClientPool
Init
Get
Insert
如果报错的话就不再插入连接池了
* 5 Server
SetConnectedHandler
SetDisconnectedHandler
Serve
* 6 TcpConnection
AsyncRead
AsyncReadAny
AsyncWrite
WriteBufferPush
ReadBufferPopHead
Finish
1 调用以后会设置连接状态不可用，再进行AsyncWrite(AsyncRead(Any)没做，TODO)会直接回调报错
2 多次调用Finish无效
3 取消AsyncWrite回调，如果AsyncWrite还没被触发会调用一次该回调
4 取消所有这个连接上的网络事件
最后调用创建的TcpConnection的Disconnected回调
```

### 3.1 可复用对象
TcpConection的创建过程不由用户控制，因此不在讨论范围
以下不可复用：
Timer的AsyncWait
Client的AsyncConnect
ClientPool的Init

### 3.2 多线程安全
1 EventLoop  
Stop，加锁调用  
2 Timer  
AsyncWait利用了Asyncer来放入循环，在AsyncWait和Cancel都加了锁来保护Asyncer  
3 Client  
SetConnectedHandler,SetDisconnectedHandler,必须在AsyncConnect调用前调用(通过flag判断)，  
因此没有放入循环，为了保护flag和handler的原子性加了锁防止跨线程调用  
AsyncConnect使用Asyncer放入循环，加锁来保护了Asyncer  
GetTcpConnection因为必须再连接建立以后才调用，不存在读写并发情况，因此没有加锁也没有使用Asyncer  
4 ClientPool全部加了锁  
主要是考虑Get和Insert操作在主线程进行时，会和sheep线程自动重连操作ClientPool所冲突
具体的验证看3.2.1
5 Server  
SetConnectedHandler,SetDisconnectedHandler同Client，在Serve前调用  
Serve使用Asyncer放入循环，加锁来保护了Asyncer  
6 TcpConnection  
AsyncRead,AsyncReadAny目前非线程安全（由于调用过程目前都在sheep loop，因此TODO）  
AsyncWrite使用Asyncer放入循环，加锁来保护了Asyncer  
WriteBufferPushBack,ReadBufferPopHead,Finish和writeHandler,readHandler加锁，保证主线程的执行和sheep线程的安全性  
PS:
1 Client和TcpConnection是一对一关系，因此Client设置的回调函数没有TcpConnection参数  
Server和TcpConnection是一对多关系，因此Server的连接回调会有TcpConnection参数(TODO，那么是否disconnect也需要加上？)  
2 TcpConnection只能以引用方式存在，其生命周期由Client决定，是为了防止用户无意识延长生命周期。  
3 Client,Timer,TcpConnection的Async回调如果在调用之前已经发生了，会立刻进行调用  
AsyncWait没有任何特殊操作，本身时间的有序性就能保证执行到handler  
AsyncRead是判断Buffer的未读长度是否满足expectSize  
AsyncWrite是调用以后才设置的epoll，因此不可能出现先发生的情况  

#### 3.2.1 ClientPool使用的多线程安全分析

仅仅讨论主线程+sheep线程，如果线程太多太复杂了

```
A 主线程解析错误(小几率因为timeout)，执行Finish
B sheep线程TcpConnection作废或者timeout（对面断开连接），执行Finish

1 主线程ClientPool获取Client
2 主线程Client获取TcpConnection，TcpConnection执行AsyncWrite，如果有错只能执行事件3
3 主线程AsyncWrite回调执行Insert，一定是发生了故障带err，TcpConnection会执行Finish，然后不执行后面事件
4 sheep线程AsyncWrite回调执行Insert，如果带err，TcpConnection会执行Finish，然后不执行后面事件
5 sheep线程Client获取TcpConnection，TcpConnection执行AsyncRead(Any)
6 sheep线程AsyncRead(Any)回调执行Insert，如果带err，TcpConnection会执行Finish，然后不执行后面事件

事件A,B会穿插在1-6任何一个步骤中，3和4互斥。3且只能是(A|B)出现在3之前导致的

然后要排除一些同线程被同线程事件打断的可能性
12 45之间都不能被同线程打断
A不能在12之间，排除1A23,1A24
B不能出现在45之间，排除124B56

然后排除加锁互斥
TcpConnection的AsyncWrite回调和Finish互斥
A不能出现在45之间，排除124A56

然后排除出错只能执行事件，A或B在2前出错，2后只能是3，排除1B24

也就是正常流程，12456
A发生在2前，(1A23),(1A24)
B发生在2前，1B23,(1B24)
A发生在3,4前，12A3,12A4
B发生在3,4前，12B3,12B4
在这之后的因为124正常，不再出现事件3
A发生在4,5,6之间，(124A56),1245A6,12456A
B发生在4,5,6之间，(124B56),1245B6,12456B
一共10种

时间线可能性列举
* 12456 
正常流程
* 1B23
B标记了TcpConnection为作废的，2执行以后，3回调执行Insert带err
* 12A3,12B3
asyncer还未投递到sheep线程，asyncer被取消，执行AsyncWrite回调，执行Insert带err
* 12A4,12B4
asyncer已经投递成功，AsyncWrite回调还没在looper中成功执行，执行AsyncWrite回调带err，执行Insert带err
* 1245A6
常规逻辑主线程不应该在56之间产生A事件
如果产生了，执行A以后，事件全部被取消，也不会再出现事件6，因此目前会导致事件泄露
* 12456A
常规逻辑主线程不应该在6之后产生A事件，一般是B事件
如果产生了，执行A以后，事件全部被取消，不再执行AsyncReadAny回调（因为该回调一般不止一次）。
不会有什么问题。
* 1245B6
B对面断开连接，接着6执行了AsyncRead回调，执行Insert带err
* 12456B
B解析错误，事件全部被取消，不再执行AsyncReadAny回调(因为该回调一般不止一次)，
TcpConnection只执行了一次Finish操作（上面都是两次）
```

## 4 TODO|FIXME
1 T EPOLLERR, EPOLLHUP
2 OK F epoll stale event
3 T asyncer with RAII
4 T buffer zero copy
5 T buffer delete unuse read data
6 F Buffer split into read write buffer
7 F GetTcpConnection if need lock
8 OK F client pool thread unsafe
9 OK F 即使server不开,client也能connect success
10 OK F client无法复用，断开连接后connect直接core
11 OK F timer迅速调用两次，并且是相同时间，会导致调用失败
12 OK F client-server的流量异常低，并且server端报reset by peer，应该是数据没读写完就close了
13 F ClientPool还连接的时候需要清理连接信息
14 F TcpConnection非线程安全bug
15 F Asyncer添加了Cancel方法，需要排查是否有需要用这个方法的地方
