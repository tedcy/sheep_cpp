#pragma once
#include <string>
#include <functional>
#include <vector>
#include <memory>

namespace small_watcher{

/*class WatcherCtxI{
public:
    WatcherCtxI() = default;
    virtual ~WatcherCtxI() = default;
    virtual void errMsg(std::string &errMsg);
};*/
class WatcherI{
public:
using onNotifyFunc = std::function<void(const std::string &errMsg)>;
using onListFunc = std::function<void(const std::string &errMsg, uint64_t afterIndex,
        std::shared_ptr<std::vector<std::string>> keys)>;
using onListWatchFunc = std::function<void(const std::string &errMsg, bool &stop, 
        std::shared_ptr<std::vector<std::string>> keys)>;
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
    virtual void List(const std::string &path, onListFunc func) = 0;

    virtual void WatchOnce(const uint64_t afterIndex, const std::string &path, onNotifyFunc func) = 0;
    virtual void CreateEphemeral(const std::string &path, const std::string &value) = 0;
    virtual void ListWatch(const std::string &path, const onListWatchFunc &func) = 0;
    //void CreateEphemeralInOrder(WatcherCtxI &ctx,
    //        const std::string &path, const std::string &value);
};
}

