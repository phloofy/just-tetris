#include <kernel/idt.hpp>
#include <kernel/types.hpp>

extern u32 idt[][2];
extern u32 kernel_cs;

namespace IDT
{

void init()
{
}

void register_interrupt(int index, uptr handler)
{
    idt[index][0] = (kernel_cs << 16) | (handler & 0x0000ffff);
    idt[index][1] = (handler & 0xffff0000) | (1 << 15) | (0xE << 8);
}

void register_trap(int index, uptr handler, u32 dpl)
{
    idt[index][0] = (kernel_cs << 16) | (handler & 0x0000ffff);
    idt[index][1] = (handler & 0xffff0000) | (1 << 15) | (dpl << 13) | (0xF << 8);
}

}
