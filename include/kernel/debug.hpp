#pragma once

#include <kernel/printf.hpp>

class Debug
{
    void printf(const char* format)
    {
    }

    template<class... Args>
    void printf(const char* format, Args... args)
    {
    }

    
private:
    const char name;
};
