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
* 4 ClientPool
Init
Get
Insert
* 5 Server
SetConnectedHandler
SetDisconnectedHandler
Serve
* 6 TcpConnection
AsyncRead
AsyncReadAny
AsyncWrite
WriteBufferPushBack
ReadBufferPopHead
```

### 多线程安全
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
5 Server
SetConnectedHandler,SetDisconnectedHandler同Client，在Serve前调用
Serve使用Asyncer放入循环，加锁来保护了Asyncer
6 TcpConnection
AsyncRead,AsyncReadAny,WriteBufferPushBack,ReadBufferPopHead目前非线程安全（TODO）
AsyncWrite使用Asyncer放入循环，加锁来保护了Asyncer
PS:
1 Client和TcpConnection是一对一关系，因此Client设置的回调函数没有TcpConnection参数  
Server和TcpConnection是一对多关系，因此Server的连接回调会有TcpConnection参数(TODO，那么是否disconnect也需要加上？)  
2 TcpConnection只能以引用方式存在  
3 Client,Timer,TcpConnection的Async回调如果在调用之前已经发生了，会立刻进行调用  
AsyncWait没有任何特殊操作，本身时间的有序性就能保证执行到handler  
AsyncRead是判断Buffer的未读长度是否满足expectSize  
AsyncWrite是调用以后才设置的epoll，因此不可能出现先发生的情况  
4 ClientPool对象设计为不可复用以减少复杂程度，假如Init失败了再次Init必定会失败

4 TODO|FIXME
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
