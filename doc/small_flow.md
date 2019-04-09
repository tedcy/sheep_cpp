## 责任链模式

small_flow是我设计的一个针对异步逻辑的责任链模式框架

设计思路概括一下:

* 将单个复杂的请求流程flow拆分为多个处理策略ploy，依次执行多个ploy。

* 当出现异步调用中断了flow时，在异步回调中调用flow会继续完成后面的流程。

通过这种方式，可以很轻易的在flow中插入新添加的同步异步ploy逻辑，而不会对前后ploy造成影响。

### 一 基本功能

#### 1.1 定义数据结构

参见[small_flow/test/service.h](https://github.com/tedcy/sheep_cpp/blob/v1.3/src/extends/small_flow/test/service.h)

使用者需要定义4个数据结构：

* Req 请求经过处理后的结果，一般在ReqFilling阶段以后就应该是只读的

* Resp 将会返回的数据中间结果，一般会在RespFilling阶段填充到pb的代码中里面去

* Task 单次请求的上下文信息

需要small_flow::FlowCtx，这是请求流程flow的上下文信息，记录了ploy执行到了哪一步

需要刚才提到的Req, Resp

需要下面的TestService，用于异步回调中调用Run来完成整个flow流程

需要std::shared\_ptr<GrpcServiceClientTest> client\_(如果是http和redis客户端则相应是他们的代码)，用于保存客户端的生命周期

* TestService 继承而来的Service实现

需要覆盖Init方法，在这里面调用SetOnMessage函数设置Server收到请求后的回调

需要覆盖Name方法，返回这个service的名称

#### 1.2 使用框架

参见[small_flow/test/main.cpp](https://github.com/tedcy/sheep_cpp/blob/v1.3/src/extends/small_flow/test/main.cpp)

```
test_service::TestService service;
//1 调用刚才定义的初始化方法
service.Init();

//2 添加ploy
service.AddPloy(new (test_service::ploy::ReqFilling));
service.AddPloy(new (test_service::ploy::ReqForward));
service.AddPloy(new (test_service::ploy::RespFilling));

//3 将service注册到server上去
server.Register(&service);

//4 运行server
server.Run();
```

#### 1.3 定义ploy

一般来说ploy至少会有两个流程，ReqFilling和RespFilling

同步的ploy都很好定义

参见[small_flow/test/req_filling.h](https://github.com/tedcy/sheep_cpp/blob/v1.3/src/extends/small_flow/test/req_filling.h)

```
class ReqFilling: public small_flow::Ploy<GrpcServiceWithTestEvent>,
    public small_packages::noncopyable {
public:
    void Run(GrpcServiceWithTestEvent &event, void *req, void *resp) override {
        Req *realReq = static_cast<Req*>(req);
        realReq->name = event.req_.name();
    }
};
```

继承了small_flow::Ploy以后，覆盖Run函数解析req和resp即可

异步的ploy需要额外关注一下

参见[small_flow/test/req_forward.h](https://github.com/tedcy/sheep_cpp/blob/v1.3/src/extends/small_flow/test/req_forward.h)

```
//在event中获取到task
auto task = event.GetTask();
//通过channel参数创建一个客户端
task->client_ = event.GetGrpcClient<helloworld::HelloRequest,
                helloworld::HelloReply, helloworld::Greeter>(channel);
//调用DoReq函数创建事件
...
//创建完毕后设置suspend为true来挂起当前flow
task->ctx_.suspend = true;
```

```
DoReq异步回调
//判断event是否失效
auto serviceEvent = client.GetServiceEvent().lock();
if (!serviceEvent) {
    return;
}
//获取task
auto task = serviceEvent->GetTask();
//执行逻辑
...
//执行完毕后调用service_->Run继续执行下面的ploy
task->service_->Run(*serviceEvent, &task->req_, &task->resp_);
```

### Trace功能

Trace是基于访问者模式，将ploy类们共用的debug逻辑外置到一个专门的TraceVisitor，

从而不会影响到核心业务逻辑的设计

```
//trace.h
//要用的ploy的前置声明
class UnitFilling;
class TraceVisitor{
public:
    static void Visit(small_flow::Ploy<GrpcBidServiceEvent> &e, void *req, void *resp) {
    }
    static void Visit(UnitFilling &e, void *req, void *resp) {
        //trace逻辑
        ...
    }
};
```

```
//unit_filling.h, ploy不再直接继承small_flow::Ploy
class UnitFilling: public small_flow::PloyVisitable<GrpcBidServiceEvent, UnitFilling, TraceVisitor> {
};
```

### TODO

这个框架还是有很多欠缺的地方

1 可以从event中拿到req和resp，也可以从Ploy的Run函数中拿到req和resp

这是设计之初没有想仔细导致的，event中的req和resp应该是private的。

2 FlowCtx的suspend没有封装成函数

3 异步回调后需要手动调用Run函数来继续执行过程

这里是一开始没有设计好，应当利用RAII的方式来自动调用

4 req和resp的限制模糊

目前req和resp都可以随意填充任何数据，没有体现出req只读，resp可读写的能力来

导致业务逻辑实现的时候req和resp分界模糊，代码可读性变差

这部分设计没想好该怎么实现
