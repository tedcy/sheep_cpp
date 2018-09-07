#pragma once
#include <string>
#include <functional>
#include <vector>

namespace small_watcher{

//TODO timeout
using onNotifyFunc = std::function<void(uint64_t&, std::string &)>;
/*class WatcherCtxI{
public:
    WatcherCtxI() = default;
    virtual ~WatcherCtxI() = default;
    virtual void errMsg(std::string &errMsg);
};*/
class WatcherI{
public:
    WatcherI() = default;
    virtual ~WatcherI() = default;
    virtual void Init(std::string &errMsg) = 0;
    virtual void GetLocalIp(std::string &ip) = 0;
    //virtual void Create(const std::string &path, const std::string &value,
    //        std::function<void(WatcherCtxI &ctx)> onDone) = 0;
    //virtual void Remove(WatcherCtxI &ctx, 
    //        const std::string &path)
    
    //virtual void Read(WatcherCtxI &ctx, 
    //        const std::string path, std::string &value) = 0;
    //virtual void List(WatcherCtxI &ctx, 
    //        const std::string &path, std::vector<std::string> &keys) = 0;

    //virtual void Watch(WatcherCtxI &ctx, 
    //        const std::string &path, const onNotifyFunc &func) = 0;
    virtual void CreateEphemeral(const std::string &path, const std::string &value) = 0;
    //void CreateEphemeralInOrder(WatcherCtxI &ctx,
    //        const std::string &path, const std::string &value);
};
}

