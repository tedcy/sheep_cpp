#include "asio_net.h"

namespace small_net {
AsioNet& AsioNet::GetInstance() {
    static AsioNet instance_;
    return instance_;
}

AsioNet::~AsioNet(){
    Shutdown();
}

void AsioNet::Shutdown() {
    if (work_ != nullptr) {
        work_ = nullptr;
        ios_.stop();
        thread_->join();
    }
}

void AsioNet::Init() {
    if (thread_ != nullptr) {
        return;
    }
    thread_ = std::make_shared<std::thread>([this](){
                work_ = std::make_shared<boost::asio::io_service::work>(ios_);
                ios_.run();
            });
}

boost::asio::io_service& AsioNet::GetIos() {
    return ios_;
}
}
