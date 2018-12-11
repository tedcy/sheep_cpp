
1 生命周期关系图

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

2 内部类和函数
```
* Connector
Connect
无法重复使用，因为一旦连接上了，内部成员会转移给TcpConenction
SetNewConnectionHandler
SetConnectFailedHandler
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


3 用户可调用的类和函数

```
* EventLoop
Wait
Stop
* Client
SetConnectedHandler
SetDisconnectedHandler
AsyncConnect
GetTcpConnection
* Server
SetConnectedHandler
SetDisconnectedHandler
Serve
* Timer
AsyncWait
Cancel
* TcpConnection
AsyncRead
AsyncWrite
WriteBufferPushBack
ReadBufferPopHead
* ClientPool
Init
Get
Insert
```

PS:
2.0 Client和TcpConnection是一对一关系，因此Client设置的回调函数没有TcpConnection参数  
Server和TcpConnection是一对多关系，因此Server的连接回调会有TcpConnection参数(TODO，那么是否disconnect也需要加上？)  
2.1 TcpConnection只能以引用方式存在  
2.2 多线程安全: EventLoop的Wait是一个循环，上述的所有函数（和函数设置的回调）都在该循环进行   
例外:
EventLoop的Stop，出于延时的考虑加锁调用  
Client,Server的Set系列必须在AsyncConnect, Serve调用前调用(通过flag判断)，
因此没有放入循环，为了保护flag和handler的原子性加了锁防止跨线程调用  
Timer的AsyncWait和Cancel都加了锁  
Asyncer因为不提供外部使用，因此是非线程安全的  
GetTcpConnection因为必须再连接建立以后才调用，不存在读写并发情况，因此没有加锁也没有使用Asyncer  
2.3 Client,Timer,TcpConnection的Async回调如果在调用之前已经发生了，会立刻进行调用  
AsyncWait没有任何特殊操作，本身时间的有序性就能保证执行到handler  
AsyncRead是判断Buffer的未读长度是否满足expectSize  
AsyncWrite是调用以后才设置的epoll，因此不可能出现先发生的情况  

4 TODO|FIXME
1 T EPOLLERR, EPOLLHUP
2 OK F epoll stale event
3 T asyncer with RAII
4 T buffer zero copy
5 T buffer delete unuse read data
6 F Buffer split into read write buffer
7 F GetTcpConnection if need lock
8 F client pool thread unsafe
9 OK F 即使server不开,client也能connect success
10 OK F client无法复用，断开连接后connect直接core
11 F timer迅速调用两次，并且是相同时间，会导致调用失败
12 OK F client-server的流量异常低，并且server端报reset by peer，应该是数据没读写完就close了
