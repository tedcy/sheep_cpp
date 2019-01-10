#include "asyncer.h"
#include "asyncer_poller.h"
#include "event.h"
#include "log.h"

namespace sheep{
namespace net{
Asyncer::Asyncer(EventLoop &loop) :
    loop_(loop){
}

Asyncer::~Asyncer() {
    //LOG(DEBUG) << "~Asyncer " << this;
    if (!done_ && handler_) {
        handler_("Asyncer Canceled");
    }
}

//any thread
void Asyncer::AsyncDo(asyncerHandlerT handler) {
    if (used_) {
        handler("Asyncer can't reuse");
        return;
    }
    used_ = true;
    done_ = false;
    Cancel();
    handler_ = handler;
    event_ = std::make_shared<Event>(loop_, AsyncerPollerFactory::Get()->GetPollerType(), 0);
    std::weak_ptr<Asyncer> weakThis = shared_from_this();
    event_->SetReadEvent([this, weakThis](){
        //loop thread
        auto realThis = weakThis.lock();
        if (!realThis) {
            //LOG(WARNING) << "Asyncer has been destoryed";
            return;
        }
        event_->DisableReadNotify();
        if (!done_) {
            done_ = true;
            handler_("");
        }
    });
    event_->EnableReadNotify();
}

void Asyncer::Cancel() {
    //LOG(DEBUG) << "Asyncer Cancel " << this;
    if (event_ == nullptr) {
        return;
    }
    event_->DisableReadNotify();
    if (!done_) {
        done_ = true;
        handler_("Asyncer Canceled");
    }
}
}
}
