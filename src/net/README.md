
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

2 用户可调用的类和函数

```
* EventLoop
Wait
Stop
* Client
SetConnectedHandler
SetDisconnectedHandler
AsyncConnect
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
```

PS:
2.1 TcpConnection只能以引用方式存在  
2.2 多线程安全: EventLoop的Wait是一个循环，上述的所有函数（和函数设置的回调）都在该循环进行   
例外:
EventLoop的Stop，出于延时的考虑加锁调用  
Server的Set系列必须在Serve调用前调用(通过flag判断)，因此没有放入循环，为了保护flag和handler的原子性加了锁防止跨线程调用  
Client的SetDisconnectedHandler必须在AsyncConnect调用前调用（通过flag判断），也没有放入循环，和AsyncConnect加了锁防止跨线程调用  
Timer的AsyncWait和Cancel都加了锁，
Ayncer因为不提供外部使用，因此是非线程安全的
2.3 Client,Timer,TcpConnection的Async回调如果在调用之前已经发生了，会立刻进行调用  
Client的SetConnectedHandler的实现是设置了一个flag，如果已经发生了设置flag状态。再次调用的时候检查flag状态决定是否要直接调用handler。  
AsyncWait没有任何特殊操作，本身时间的有序性就能保证执行到handler  
AsyncRead是判断Buffer的未读长度是否满足expectSize  
AsyncWrite是调用以后才设置的epoll，因此不可能出现先发生的情况  

3
