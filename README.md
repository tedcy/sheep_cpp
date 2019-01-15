sheep库是一个基于grpc的c++服务化框架

sheep库的[go版本](https://github.com/tedcy/sheep)在这里

和go版本相比目前没有包含治理功能

- [1 功能](#1-)
- [2 安装](#2-)
- [3 使用](#3-)
    - [3.1 small-client](#31-small-client)
        - [3.1.1 http-client](#311-http-client)
        - [3.1.2 redis-client](#312-redis-client)
    - [3.2 small-server](#32-small-server)
        - [3.2.1 grpc-client](#321-grpc-client)
        - [3.2.2 grpc-server](#322-grpc-server)
        - [3.2.3 client-in-grpc-server](#323-client-in-grpc-server)
        - [3.2.4 服务端在etcd上添加临时节点](#324-etcd)
        - [3.2.5 客户端使用etcd服务发现](#325-etcd)
- [4 参考例子](#4-)
- [5 TODO list](#5-todo-list)

## 1 功能
* grpc的回调风格封装（原始风格烧脑且不好用，原始风格说明文档TODO）
* 同一个grpc-server上多个grpc-service
* 异步grpc客户端
* 异步http客户端
* 异步redis客户端
* 依赖etcd的服务化管理

**注意：目前proto文件需要约定接口名称**

可复用基础库
* sheep::net  
基于epoll的异步网络库，用作grpc服务以外的网络实现   
* small_http_parser  
http解析库，依赖http-parser  
* small_test  
单元测试库，依赖gtest，纯下载安装脚本  
* small_hiredis  
redis客户端，依赖hiredis，纯下载安装脚本  
* small_grpc  
grpc库的下载安装脚本  
* small_packages  
包含一些常用的string,lock,time,block_queue等封装  

extends扩展库
* log  
对glog的封装，解决glog打印日志core问题  
* small_config  
解析配置，对toml库的封装  
* small_watcher  
异步etcd客户端实现  
* small_pprof  
性能测试debug封装，依赖gperf  
* small_flow  
责任链模式下对small\_server的封装，适合单个复杂逻辑的处理  

## 2 安装

```
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh
./vcpkg install glog tinytoml rapidjson
cd -

git clone https://github.com/tedcy/sheep_cpp
cd sheep_cpp
vcpkg_path=xxx ./bootstrap.sh
```

sheep使用cmake管理依赖关系，使用sheep的项目在CMakeLists.txt中加入以下

```
set(CMAKE_MODULE_PATH ${SHEEP_PROJECT_SOURCE_DIR}/cmake)

find_package(small_flow REQUIRED)
include_directories(${TARGET} ${small_flow_INCLUDE_DIRS})
target_link_libraries(${TARGET} ${small_flow_LIBRARIES})

find_package(small_watcher REQUIRED)
include_directories(${TARGET} ${small_watcher_INCLUDE_DIRS})
target_link_libraries(${TARGET} ${small_watcher_LIBRARIES})

find_package(small_config REQUIRED)
include_directories(${TARGET} ${small_config_INCLUDE_DIRS})
target_link_libraries(${TARGET} ${small_config_LIBRARIES})
```

```
然后cmake指定SHEEP_PROJECT_SOURCE_DIR为sheep_cpp git clone的路径
cmake -DSHEEP_PROJECT_SOURCE_DIR=xxx
```

## 3 使用

### 3.1 small-client

small-client是一个异步客户端的基础库，包含了异步的http和redis客户端。  

用户需要操作的类有三个，Looper，ClientChannel，Client    
* Looper  
事件循环的抽象，由于grpc库的限制，目前暂时都是单例    
* ClientChannel  
客户端连接池的抽象，small\_client的是ClientChannel以及下面small\_server的是GrpcClientChannel  
都需要传入Looper参数。  
所有的Client都会从ClientChannel中拿出连接对象进行通信  
和服务端相关的配置参数都需要在这里设定  
* Client  
各种客户端small\_client::HttpClient, small\_client::RedisClient，以及下面small\_server的GrpcClient。  
都至少需要传入ClientChannel参数  
和本次通信相关的配置参数都需要在这里设定  

#### 3.1.1 http-client

Looper,ClientChannel部分:

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
//判断errMsg
```

Client部分：

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
    //判断errMsg
    
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

#### 3.1.2 redis-client

```
//Looper,ClientChannel部分同http-redis
//...

//使用channel创建一个redis-client
auto client = std::make_shared<small_client::RedisClient>(channel);

//SetTimeoutMs可选项来添加超时时间

//"GET A"是具体的redis命令
client->DoReq("GET A", 
    [](small_client::RedisClient &client, const std::string &errMsg) {
    //...
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

Looper,ClientChannel部分:

```
//初始化grpc事件循环单例
small_server::GrpcLooper::GetInstance()->Init();

//初始化Greeter的grpc Channel
small_server::GrpcClientChannel<helloworld::Greeter> channel;
channel.SetResolverType("string");
channel.Init(errMsg, {"127.0.0.1"}, 8888, "");
```

Client部分：
```
创建GrpcClient分别需要传入Request,Response以及Service部分的名称
using GrpcClientTest = small_server::GrpcClient<helloworld::HelloRequest,
        helloworld::HelloReply, helloworld::Greeter>;
auto client = std::make_shared<GrpcClientTest>(channel);
client->Init();
client->req_.set_name("proxy");
client->DoReq([](GrpcClientTest &client, const std::string &errMsg) {
    LOG(INFO) << client.resp_.message();
});
```

#### 3.2.2 grpc-server

有service和server概念，service指服务，server指服务器  
每个server单独一个listen，多路复用不同的service  

service部分
```
//前置声明service和task
class TestService;
struct TestTask;

//使用request,response,service::AsyncService,task声明service类型
using GrpcServiceWithTest = small_server::GrpcService<
    helloworld::HelloRequest, helloworld::HelloReply,
    helloworld::Greeter::AsyncService, TestTask>;

//声明service上下文信息
using GrpcServiceWithTestEvent = GrpcServiceWithTest::GrpcServiceEvent;

//task定义
struct TestTask {
};

//继承上面定义的service类型
class TestService: public GrpcServiceWithTest{
public:
    //设置回调
    void Init() {
        auto onMessage = [this](GrpcServiceWithTestEvent &event,
            std::string &errMsg) {
            //判断errMsg
            //获取service事件中的req信息
            auto name = event.req_.name();

            //获取service事件的task信息
            auto task = event.GetTask();

            //设置返回值
            event.resp_.set_message("from server");

            //结束请求
            event.Finish();
            return;
        };
        SetOnMessage(onMessage);
    }
    std::string Name() override {
        return "test";
    }
};
```

server部分
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

#### 3.2.3 client-in-grpc-server

在service上转发请求到下一个服务端  
在grpc-server的service基础上修改  
相对上面的各个独立client，Looper部分不需要设置，ClientChannel部分相同  

Client部分
```
struct TestTask {
    std::shared_ptr<small_client::HttpClient> client;
};
client = event.GetHttpClient(channel,
            "GET", "/", "");
client->DoReq([](small_client::HttpClient &client, const std::string &errMsg) {
    
    //客户端的异步回调中获取到service的上下文信息
    auto serviceEvent = client.GetServiceEvent().lock();
    if (!serviceEvent) {
        return;
    }
    
    //service返回结果
    serviceEvent->resp_.set_message(client.GetRespStr());
    
    //service关闭
    serviceEvent->Finish();
});
```

直接从service获取的event部分GetHttpClient，GetRedisClient，GetGrpcClient即可  
返回对应的智能指针，用法和独立客户端相同  

#### 3.2.4 服务端在etcd上添加临时节点

```
//初始化Looper
small_client::Looper::GetInstance()->Init();

//创建watcher
auto watcher = small_watcher::MakeWatcher({"172.16.187.149"}, 2379);
watcher->Init(errMsg);

//判断errMsg
std::string ip;
watcher->GetLocalIp(ip);
watcher->CreateEphemeral("/test/" + ip + ":8888", "");
```

#### 3.2.5 客户端使用etcd服务发现

```
//初始化Looper
small_client::Looper::GetInstance()->Init();

//将Watcher Resolver注入到Resolver管理器中
small_watcher::WatcherResolverFactory::GetInstance()->Init();

//创建任意一个channel，略

//设置resolver类型为"watcher"
channel.SetResolverType("watcher");
channel.Init(errMsg, {"172.16.187.149"}, 2379, "/test");
//检查错误
```

## 4 参考例子

[http\_client](https://github.com/tedcy/sheep_cpp/blob/master/src/small_client/test/http_client/main.cpp)

[redis\_client](https://github.com/tedcy/sheep_cpp/blob/master/src/small_client/test/redis_client/main.cpp)

[grpc\_client\_with\_etcd](https://github.com/tedcy/sheep_cpp/blob/master/src/small_server/test/grpc_client/main.cpp)

[grpc\_server\_with\_etcd](https://github.com/tedcy/sheep_cpp/blob/master/src/small_server/test/server/main.cpp)

## 5 TODO list
* 简化安装过程（去除vcpkg依赖）
* 取消部分类的单例
* 取消grpc限制service的接口名称为Handler的限制
