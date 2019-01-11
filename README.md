sheep库是一个基于grpc的c++服务化框架

sheep库的[go版本](https://git.dev.tencent.com/tedcy/sheep.git)在这里

和go版本相比目前没有包含治理功能

__TOC__

## 1 功能
* grpc的回调风格封装（原始风格烧脑且不好用，原始风格说明文档TODO）
* 同一个grpc-server上多个grpc-service
* 异步grpc客户端
* 异步http客户端
* 异步redis客户端

**注意：目前proto文件需要约定接口名称**

## 2 安装

```
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg install glog jsoncpp tinytoml rapidjson
cd -

git clone https://github.com/tedcy/sheep_cpp
cd sheep_cpp
vcpkg_path=xxx ./bootstrap.sh
```

## 3 使用

### 3.1 small-client

small-client是一个异步客户端的基础库，包含了异步的http和redis客户端。  
分为配置部分和使用部分：使用独立的客户端需要了解配置部分。  
而在服务端上调用异步客户端，对其配置过程进行了封装，只需要关心使用部分即可。  

#### 3.1.1 http-client的创建使用:

配置部分:

```
//先初始化事件循环单例
small_client::Looper::GetInstance()->Init();

//用事件循环单例创建一个ClientChannel，它代表了负载均衡和服务发现的基础管道
small_client::ClientChannel channel(small_client::Looper::GetInstance()->GetLoop());

//选择dns作为服务发现类型，默认只有dns和string
//dns会异步解析地址，并且在dns变动后更新，只用于Init的第二个参数填入http地址
//string固定读取一次，只用于Init第二个参数填入ip地址
//结合small_watcher库进行注入的话还可以使用etcd进行服务发现
channel.SetResolverType("dns");

//设置连接池大小为1
channel.SetMaxSize(1);

//填入的4个参数，分别是
//判断错误的errMsg, type std::string
//地址，服务发现类型会解析这一串地址，并且根据SetLbPolicyType选择，默认是随机, type const std::vector<std::string> &
//端口
//路径，用于专门的服务发现组件路径，例如etcd和zookeeper
channel.Init(errMsg, {"www.baidu.com"}, 80, "");
if (!errMsg.empty()) {
    LOG(FATAL) << errMsg;
}
```

使用部分：

```
//使用channel创建一个http_client
//四个参数分别是
//1 创建的channel
//2 http方法
//3 路径
//4 body内容
auto client = std::make_shared<small_client::HttpClient>(channel,
            small_http_parser::ReqFormater::MethodGET, "/", "");

//SetHeaders,SetQueryString,SetHost分别是可选项来填入http协议各项

//SetTimeoutMs可选项来添加超时时间

client->DoReq([](small_client::HttpClient &client, const std::string &errMsg) {
    
    //失败
    if (!errMsg.empty()) {
        LOG(ERROR) << errMsg;
        return;
    }
    
    //成功，获取header
    auto headers = client.GetRespHeader();
    for (auto &header: headers.kvs_) {
        LOG(INFO) << header.first << ":" << header.second;
    }
    //获取body
    auto resp = client.GetRespStr();
    LOG(INFO) << resp;
})
```

#### 3.1.2 redis-client的配置使用

配置部分和http-client相同

使用部分：
```
//使用channel创建一个redis-client
auto client = std::make_shared<small_client::RedisClient>(channel);

//SetTimeoutMs可选项来添加超时时间

//"GET A"是具体的redis命令
client->DoReq("GET A", 
    [](small_client::RedisClient &client, const std::string &errMsg) {
    
    //失败
    if (!errMsg.empty()) {
        LOG(ERROR) << errMsg;
        return;
    }
    
    //成功，获取结果
    bool ok;
    auto resp = client.GetResp(ok);
    if (!ok) {
        LOG(WARNING) << "not exist";
        return;
    }
    LOG(INFO) << resp;
});
```

### 3.2 small-server

small-server库包含了grpc-client和grpc-server，是对grpc的回调风格封装

proto文件需要约定固定接口名称"Handler"（可以修改源码定制自己想要的固定接口名称，计划以后解决这个限制）

```
syntax = "proto3";

package helloworld;

service Greeter {
  rpc Handler(HelloRequest) returns (HelloReply) {}
}

message HelloRequest {
  string name = 1;
}

message HelloReply {
  string message = 1;
  repeated Test tests = 2;
}

message Test {
    string testMessage = 1;
}
```

#### 3.2.1 grpc-client

配置部分:

```
//初始化grpc事件循环单例
small_server::GrpcCore::GetInstance()->Init();
small_server::GrpcCore::GetInstance()->Run();

//初始化Greeter的grpc Channel单例
small_server::GrpcClientCore<helloworld::Greeter>::GetInstance()->SetResolverType("string");
small_server::GrpcClientCore<helloworld::Greeter>::GetInstance()->Init(errMsg, {"127.0.0.1"}, 8888, "");

//使用部分代码:
...

//等待结束
small_server::GrpcCore::GetInstance()->Wait();
```

使用部分：
```
创建GrpcClient分别需要传入Request,Response以及Service部分的名称
using GrpcClientTest = small_server::GrpcClient<helloworld::HelloRequest,
        helloworld::HelloReply, helloworld::Greeter>;
auto client = std::make_shared<GrpcClientTest>();
client->Init();
client->req_.set_name("proxy");
client->DoReq([](GrpcClientTest &client, const std::string &errMsg) {
    LOG(INFO) << client.resp_.message();
});
```

#### 3.2.2 grpc-server

有service和server概念，service指服务，server指服务器  
每个server单独一个listen，多路复用不同的service  

定义service
```
//前置声明service和task
class TestService;
struct TestTask;

//使用request,response,service::AsyncService,task声明service类型
using GrpcServiceWithTest = small_server::GrpcService<
    helloworld::HelloRequest, helloworld::HelloReply,
    helloworld::Greeter::AsyncService, TestTask>;

//声明service上下文信息
using GrpcServiceWithTestCtx = GrpcServiceWithTest::GrpcServiceCtx;

//task定义
struct TestTask {
};

//继承上面定义的service类型
class TestService: public GrpcServiceWithTest{
public:
    //设置回调
    void Init() {
        auto onMessage = [this](GrpcServiceWithTestCtx &ctx,
            std::string &errMsg) {
            //获取上下文中的req信息
            auto name = ctx.req_.name();

            //获取上下文的task信息
            auto task = ctx.GetTask();

            //设置返回值
            ctx.resp_.set_message("from server");

            //结束请求
            ctx.Finish();
            return;
        };
        SetOnMessage(onMessage);
    }
    std::string Name() override {
        return "test";
    }
};
```

使用service
```
//创建server并且初始化
small_server::GrpcServer server("127.0.0.1:8888");
server.Init();

//创建service并且初始化
TestService service;
service.Init();

//将service注册到server上
server.Register(&service);

//等待程序结束
server.Run();
```

### 3.2.3 http-client-in-grpc-server

在grpc-server的service基础上修改

配置过程加上

```
small_client::ClientChannel channel(small_client::Looper::GetInstance()->GetLoop());
channel.SetResolverType("dns");
channel.SetMaxSize(1);
channel.Init(errMsg, {"www.baidu.com"}, 80, "");
if (!errMsg.empty()) {
    LOG(FATAL) << errMsg;
}
```

使用过程加上
```
struct TestTask {
    std::shared_ptr<small_client::HttpClient> client;
};
client = ctx.GetHttpClient(channel,
            "GET", "/", "");
client->DoReq([](small_client::HttpClient &client, const std::string &errMsg) {
    
    //客户端的异步回调中获取到service的上下文信息
    auto serviceCtx = client.GetServiceCtx().lock();
    if (!serviceCtx) {
        return;
    }
    
    //service返回结果
    serviceCtx->resp_.set_message(client.GetRespStr());
    
    //service关闭
    serviceCtx->Finish();
});
```

### 3.2.4 redis-client-in-grpc-server

在grpc-server的service基础上修改

配置过程和http-client-in-grpc-server相同

使用过程类似
```
struct TestTask {
    std::shared_ptr<small_client::RedisClient> client;
};
client = ctx.GetHttpClient(channel,
            "GET", "/", "");
client->DoReq("GET A",
    [](small_client::RedisClient &client, const std::string &errMsg) {
    
    //客户端的异步回调中获取到service的上下文信息
    auto serviceCtx = client.GetServiceCtx().lock();
    if (!serviceCtx) {
        return;
    }
    
    //成功，获取结果
    bool ok;
    auto resp = client.GetResp(ok);
    if (!ok) {
        LOG(WARNING) << "not exist";
        serviceCtx->Finish();
        return;
    }
    //service返回结果
    serviceCtx->resp_.set_message(resp);
    
    //service关闭
    serviceCtx->Finish();
});
```

### 3.2.5 grpc-client-in-grpc-server

在grpc-server的service基础上修改

配置过程加上

```
small_server::GrpcClientCore<helloworld::Greeter>::GetInstance()->SetResolverType("string");
small_server::GrpcClientCore<helloworld::Greeter>::GetInstance()->Init(errMsg, {"127.0.0.1"}, 8888, "");
```

使用过程加上
```
//task需要保存从service获取的智能指针
struct TestTask {
    std::shared_ptr<GrpcServiceClientTest> client;
};

//service onMessage回调
client = ctx.GetGrpcClient<helloworld::HelloRequest,
                    helloworld::HelloReply, helloworld::Greeter>();
client->DoReq([](GrpcServiceClientTest &client, const std::string &errMsg) {
    
    //客户端的异步回调中获取到service的上下文信息
    auto serviceCtx = client.GetServiceCtx().lock();
    if (!serviceCtx) {
        return;
    }
    
    //service返回结果
    serviceCtx->resp_.set_message(client.resp_.message());
    
    //service关闭
    serviceCtx->Finish();
});
```

## 4 TODO list
* 简化安装过程（去除vcpkg依赖）
* 取消部分类的单例
* 取消grpc限制service的接口名称为Handler的限制
