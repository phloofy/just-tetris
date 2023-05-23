#include <kernel/crt.hpp>

typedef void (*fptr)();

extern "C" fptr _init_array_start[0], _init_array_end[0];
extern "C" fptr _fini_array_start[0], _fini_array_end[0];

void CRT::init()
{
    for (fptr* func = _init_array_start; func != _init_array_end; func++)
    {
	(*func)();
    }
}

void CRT::fini()
{
    for (fptr* func = _fini_array_start; func != _fini_array_end; func++)
    {
        (*func)();
    }
}

fptr _init_array_start[0] __attribute__ ((used, section(".init_array"), aligned(sizeof(fptr)))) = {};
fptr _fini_array_start[0] __attribute__ ((used, section(".fini_array"), aligned(sizeof(fptr)))) = {};

extern "C" int __cxa_atexit(void (*func)(void*), void* arg, void* dso_handle)
{
    return 0;
}
