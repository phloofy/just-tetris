#pragma once

#include <kernel/types.hpp>

extern "C" u8 inb(int port);
extern "C" u16 inw(int port);
extern "C" u32 inl(int port);

extern "C" void outb(int port, u8 value);
extern "C" void outw(int port, u16 value);
extern "C" void outl(int port, u32 value);

extern "C" void monitor(volatile void*);
extern "C" void mwait();
extern "C" void pause();

extern "C" u64 rdmsr(u32 id);
extern "C" void wrmsr(u32 id, u64 val);

extern "C" u32 get_flags();
extern "C" bool int_disable();
extern "C" void int_restore(bool state);

struct cpuid_regs
{
    u32 eax;
    u32 ebx;
    u32 ecx;
    u32 edx;
};

extern "C" void cpuid(u32 eax, cpuid_regs* out);
