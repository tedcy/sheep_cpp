生命周期关系图

```mermaid
graph LR;
note[虚线是关联关系<br>实线是组合关系]
subgraph EventLoop
EpollEvent-.->EventLoop
TimerEvent-.->EventLoop
EpollPoller-->EventLoop
TimerPoller-->EventLoop
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
```
