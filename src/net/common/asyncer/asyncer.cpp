#include "asyncer.h"
#include "asyncer_poller.h"
#include "event.h"
#include "log.h"

namespace sheep{
namespace net{
Asyncer::Asyncer(EventLoop &loop) :
    loop_(loop) {
}

void Asyncer::AsyncDo(asyncerHandlerT handler) {
    cancel();
    handler_ = handler;
    event_ = std::make_shared<Event>(loop_, AsyncerPollerFactory::Get()->GetPollerType(), 0);
    std::weak_ptr<Event> weakEvent = event_;
    event_->SetReadEvent([weakEvent, this](){
        auto event = weakEvent.lock();
        if (!event) {
            LOG(WARNING) << "Asyncer has been destoryed";
            return;
        }
        handler_("");
    });
    event_->EnableReadNotify();
}

void Asyncer::cancel() {
    if (event_ == nullptr) {
        return;
    }
    event_->DisableReadNotify();
    handler_("Asyncer Canceled");
}
}
}
