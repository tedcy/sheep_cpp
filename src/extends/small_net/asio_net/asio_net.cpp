#include "asio_net.h"

namespace small_net {
std::shared_ptr<AsioNet> AsioNet::instance_ = nullptr;

std::shared_ptr<AsioNet> AsioNet::GetInstance() {
    if (instance_ == nullptr) {
        instance_ = std::shared_ptr<AsioNet>(new AsioNet());
    }
    return instance_;
}

AsioNet::~AsioNet(){
    work_ = nullptr;
    ios_.stop();
    thread_->join();
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
