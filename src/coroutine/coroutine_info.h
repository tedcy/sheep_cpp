#pragma once

#include <functional>
#include "tc_fcontext.h"
#include "tc_fcontext_x86_64.h"
#include "intrusive_list.h"

class CoroutineInfo : public IntrusiveListNode<CoroutineInfo> {
public:
    static void initContext(intptr_t v);
    void registerFunc(const std::function<void()>& func,
                      const std::function<void()>& endFunc);
    static void switchTo(CoroutineInfo *to);
    static void switchBack();
private:
    std::function<void()> func_;
    std::function<void()> endFunc_;
    fcontext_t* ctx_;
    fcontext_t ctxFrom_;
};