#include "tc_fcontext.h"
#include <iostream>

using namespace std;

#define DEFAULT_SIZE 128 * 1024

struct stack_context {
    std::size_t    size = 0;
    void*        sp = 0;
};

#include <unistd.h>
std::size_t pagesize() {
    static std::size_t size = ::sysconf( _SC_PAGESIZE);
    return size;
}

#include <cstdlib>
stack_context default_stack() {
    stack_context ctx;
    ctx.size = DEFAULT_SIZE;
    ctx.sp = ::aligned_alloc(pagesize(), ctx.size);
    return ctx;
}

static void foo(intptr_t v) {
    asm(".cfi_undefined rip");

    fcontext_t *mainContext = (fcontext_t*)v;
    fcontext_t nouse;

    cout << "foo" << endl;

    jump_fcontext(&nouse, mainContext, 0, false);
}

int main() {
    auto stack = default_stack();
    auto ctx_to = make_fcontext(stack.sp, stack.size, foo);
    fcontext_t mainContext;

    jump_fcontext(&mainContext, ctx_to, intptr_t(&mainContext), false);

    cout << "finished" << endl;
}
