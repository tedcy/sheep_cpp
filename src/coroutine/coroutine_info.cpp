#include "coroutine_info.h"
#include "tc_fcontext.h"
#include "coroutine_scheduler.h"
#include <cstdint>
#include <assert.h>
#include <iostream>

#define DEFAULT_SIZE 128 * 1024

struct stack_context {
    std::size_t size = 0;
    void *sp = 0;
};

#include <unistd.h>
static std::size_t pagesize() {
    static std::size_t size = ::sysconf( _SC_PAGESIZE);
    return size;
}

#include <cstdlib>
static stack_context default_stack() {
    stack_context ctx;
    ctx.size = DEFAULT_SIZE;
    ctx.sp = ::aligned_alloc(pagesize(), ctx.size);
    return ctx;
}

void CoroutineInfo::initContext(intptr_t v) {
    asm(".cfi_undefined rip");

    CoroutineInfo *info = (CoroutineInfo*)v;

    std::cout << "initContext" << std::endl;
    
    jump_fcontext(info->ctx_, &info->ctxFrom_, 0, false);

    try {
        info->func_();
    }catch (...) {
        std::cout << "exception" << std::endl;
    }

    std::cout << "initContext func end" << std::endl;

    info->endFunc_();

    jump_fcontext(info->ctx_, &info->ctxFrom_, 0, false);
    
    assert(false);
}

void CoroutineInfo::registerFunc(const std::function<void()> &func,
                                 const std::function<void()> &endFunc) {
    func_ = func;
    endFunc_ = endFunc;

    auto stack = default_stack();
    ctx_ = make_fcontext(stack.sp, stack.size, initContext);

    std::cout << "registerFunc" << std::endl;

    jump_fcontext(&ctxFrom_, ctx_, intptr_t(this), false);
    
    std::cout << "registerFunc end" << std::endl;
}

void CoroutineInfo::switchTo(CoroutineInfo *to) {
    CoroutineScheduler::currentCoro() = to;

    std::cout << "switchTo|" << to << std::endl;

    jump_fcontext(&to->ctxFrom_, to->ctx_, intptr_t(to), false);
    
    std::cout << "switchToEnd|" << to << std::endl;
}

void CoroutineInfo::switchBack() {
    auto cur = CoroutineScheduler::currentCoro();
    
    std::cout << "switchBack|" << cur << std::endl;

    jump_fcontext(cur->ctx_, &cur->ctxFrom_, 0, false);
    
    std::cout << "switchBackEnd|" << cur << std::endl;
}