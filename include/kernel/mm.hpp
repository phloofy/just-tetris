#pragma once

#include <kernel/types.hpp>

extern "C" void* kmem_alloc(usize size);
extern "C" void kmem_free(void* ptr);

extern void kmem_init(uptr start, uptr end);
