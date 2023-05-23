#pragma once

#include <kernel/types.hpp>

namespace IDT
{

void init();
void register_interrupt(int index, uptr handler);
void register_trap(int index, uptr handler, u32 dpl);

}
