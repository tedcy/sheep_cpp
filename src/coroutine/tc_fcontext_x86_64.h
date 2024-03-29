#ifndef _TC_CONTEXT_FCONTEXT_X86_64_H
#define _TC_CONTEXT_FCONTEXT_X86_64_H

#include <cstdint>
#include <cstddef>

extern "C" 
{

struct fc_stack_t
{
    void* sp;
    std::size_t size;

    fc_stack_t()
    : sp(0)
    , size(0)
    {}
};

struct fp_t
{
    uint32_t fc_freg[2];

    fp_t()
    : fc_freg()
    {}
};

struct fcontext_t
{
    uint64_t fc_greg[8];
    fc_stack_t fc_stack;
    fp_t fc_fp;

    fcontext_t()
    : fc_greg()
    , fc_stack()
    , fc_fp()
    {}
};

}

#endif
