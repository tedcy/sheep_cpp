#sheep_cpp

## 1 simple_io是最简单的版本  
## 2 complicated_io是尝试支持文件读写，服务端新建客户端请求的版本  
```
事件回调序列  
connection_server:
eventloop->fileevent->event->service
```
eventloop封装了fileevent，对外部来说fileevent是不可见的。  
event是利用buffer对读写操作的封装，使得外部不需要裸读写，直接操作buffer即可  
service抽象了添加读写事件接口，通过继承体系让人重写message回调，close回调  

缺点
1 对socket没有raii化，导致对裸socket的操作遍布代码的各个部分  
2 对acceptor和connector的概念没有抽象，service耦合了acceptor代码  
3 event概念不好，将每次tcp的读写单独化了，一次proxy行为，需要建立四个event。用tcpconection来描述只需要两个对象  
4 service概念是业务层和server层中间的东西，是业务协议层对server的多路复用。不放在网络层
引入protobuf以后可以做一层service
5 由于service和server层以及event分工混乱，导致写client代码不好写，陷入死路
6 太多include导致内部类被暴露到外部来

## 3 complicated_io_refactoring是参考别人写的框架，尝试重构的版本  
TODO:  
1 将eventloop,fileevent分拆为eventloop,poller,event(原fileevent)三部分，
不再隐藏event（原fileevent）部分，外部可以直接使用(但对用户隐藏)  
2 将event改为tcpconnection  
```
事件回调序列
acceptor:
eventloop->poller->event->acceptor->server->createNewConnection(update event)
connection_server:
eventloop->poller->event->tcpconnection->server
connection_client:
connector->createNewConnection(update event)
eventloop->poller->event->connector->server
```
