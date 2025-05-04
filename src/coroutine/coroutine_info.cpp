#include "coroutine_info.h"
#include "tc_fcontext.h"
#include "coroutine_scheduler.h"
#include <cstdint>
#include <assert.h>
#include "log.h"

using namespace std;

#ifdef ADDRESS_SANITIZER

//[support swapcontext](https://github.com/google/sanitizers/issues/189)
//切换协程之前先start，切换之后finish
// start
//  第一个参数：保存当前stack
//  后续参数：切换的目标协程context的sp和size，可以取make_fcontext或者jump_fcontext存储的值，
//      也可以使用__sanitizer_finish_switch_fiber保存下来的值，
//      我使用的是xxx_fcontext值
// finish
//  第一个参数：恢复stack，如果第一次切换过来就传null，否则填之前存储的fake_stack地址
//  后续参数：保存切换之前的context的sp和size，用于start使用，可以传null
//      我start使用了xxx_fcontext值，所以这里传null

static void __sanitizer_start_switch_fiber(void **fake_stack_save,
                                           const void *stack_bottom,
                                           size_t stack_size)
    __attribute__((__weakref__("__sanitizer_start_switch_fiber")));
static void __sanitizer_finish_switch_fiber(void *fake_stack_save,
                                            const void **stack_bottom_old,
                                            size_t *stack_size_old)
    __attribute__((__weakref__("__sanitizer_finish_switch_fiber")));
#endif

#define DEFAULT_SIZE 128 * 1024

#include <unistd.h>
static std::size_t pagesize() {
    static std::size_t size = ::sysconf( _SC_PAGESIZE);
    return size;
}

struct stack_context {
    std::size_t size = 0;
    void *sp = 0;
    void zero() {
#ifdef ADDRESS_SANITIZER
        std::fill_n(static_cast<char *>(sp) - size + pagesize(),
                    size - pagesize(), 0);
#endif
    }
};

#include <cstdlib>
static stack_context default_stack() {
    stack_context ctx;
    ctx.size = DEFAULT_SIZE;

    //sp表示栈顶指针, 栈由高地址向低地址生长，所以sp的初始值指向栈底位置，也就是分配内存的最大值，需要加上size
    ctx.sp = static_cast< char * >(::aligned_alloc(pagesize(), ctx.size)) + ctx.size;
    return ctx;
}

void CoroutineInfo::initContext(intptr_t v) {
    asm(".cfi_undefined rip");

    CoroutineInfo *info = (CoroutineInfo*)v;
    
#ifdef ADDRESS_SANITIZER
    __sanitizer_finish_switch_fiber(nullptr, nullptr, nullptr);
#endif

    LOG(INFO) << "initContext" << endl;
    
#ifdef ADDRESS_SANITIZER
    void *fake_stack;
    __sanitizer_start_switch_fiber(&fake_stack, info->ctx_->fc_stack.sp,
                                   info->ctx_->fc_stack.size);
#endif
    
    jump_fcontext(info->ctx_, &info->ctxFrom_, 0, false);

#ifdef ADDRESS_SANITIZER
    __sanitizer_finish_switch_fiber(fake_stack, nullptr, nullptr);
#endif

    try {
        info->func_();
    }catch (...) {
        LOG(ERROR) << "exception" << endl;
    }

    LOG(INFO) << "initContext func end" << endl;

    info->endFunc_();

#ifdef ADDRESS_SANITIZER
    __sanitizer_start_switch_fiber(&fake_stack, &info->ctxFrom_.fc_stack.sp,
                                   info->ctxFrom_.fc_stack.size);
#endif

    jump_fcontext(info->ctx_, &info->ctxFrom_, 0, false);
    
    assert(false);
}

void CoroutineInfo::registerFunc(const std::function<void()> &func,
                                 const std::function<void()> &endFunc) {
    func_ = func;
    endFunc_ = endFunc;

    auto stack = default_stack();
    
    stack.zero();
    
    ctx_ = make_fcontext(stack.sp, stack.size, initContext);

    LOG(INFO) << "registerFunc" << endl;

#ifdef ADDRESS_SANITIZER
    void *fake_stack;
    __sanitizer_start_switch_fiber(&fake_stack, ctx_->fc_stack.sp,
                                   ctx_->fc_stack.size);
#endif

    jump_fcontext(&ctxFrom_, ctx_, intptr_t(this), false);

#ifdef ADDRESS_SANITIZER
    __sanitizer_finish_switch_fiber(fake_stack, nullptr, nullptr);
#endif

    LOG(INFO) << "registerFunc end" << endl;
}

void CoroutineInfo::switchTo(CoroutineInfo *to) {
    CoroutineScheduler::currentCoro() = to;

    // LOG(INFO) << "switchTo|" << to << endl;

#ifdef ADDRESS_SANITIZER
    void *fake_stack;
    __sanitizer_start_switch_fiber(&fake_stack, to->ctx_->fc_stack.sp,
                                   to->ctx_->fc_stack.size);
#endif

    jump_fcontext(&to->ctxFrom_, to->ctx_, intptr_t(to), false);
    
#ifdef ADDRESS_SANITIZER
    __sanitizer_finish_switch_fiber(fake_stack, nullptr, nullptr);
#endif
    
    // LOG(INFO) << "switchToEnd|" << to << endl;
}

void CoroutineInfo::switchBack() {
    auto cur = CoroutineScheduler::currentCoro();
    
#ifdef ADDRESS_SANITIZER
    void *fake_stack;
    __sanitizer_start_switch_fiber(&fake_stack, cur->ctxFrom_.fc_stack.sp,
                                   cur->ctxFrom_.fc_stack.size);
#endif

    jump_fcontext(cur->ctx_, &cur->ctxFrom_, 0, false);
    
#ifdef ADDRESS_SANITIZER
    __sanitizer_finish_switch_fiber(fake_stack, nullptr, nullptr);
#endif
}