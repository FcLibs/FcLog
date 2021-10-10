
#ifndef FCLOG_X86_H
#define FCLOG_X86_H
#ifdef linux

#include <stdint.h>

static inline uint64_t FcLog_MSR() {
    uint32_t eax, ebx;
    asm volatile (
    "LFENCE\n\t"
    "rdtsc\n\t"
    "movl %%eax, %0\n\t"
    "movl %%edx, %1\n\t"
    : "=rm" (eax), "=rm" (ebx)
    ::"%edx", "%eax"
    );
    return (uint64_t) ebx << 32 | eax;

}


#elif WIN32


#include <stdint.h>
#include <intrin.h>

static inline unsigned long long FcLog_MSR() {
    _mm_lfence();
    return __rdtsc();

}



#endif 
#endif //FCLOG_X86_H