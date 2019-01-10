sheep库是一个基于grpc的c++服务化框架

sheep库的[go版本](https://git.dev.tencent.com/tedcy/sheep.git)在这里

和go版本相比目前没有包含治理功能

## 功能
* 同一个grpc-server上多个grpc-service
* 异步grpc客户端
* 异步http客户端
* 异步redis客户端

## 使用

### small-client

small-client是一个异步客户端的基础库，包含了异步的http和redis客户端。  
分为创建部分和使用部分：使用独立的客户端需要了解创建部分。  
而在服务端上调用异步客户端，对其创建过程进行了封装，只需要关心使用部分即可。  

#### http-client的创建使用:

创建部分:

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

#### redis-client的创建使用

创建部分和http-client相同

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

### small-server

#### grpc-service

#### grpc-client
