#pragma once

#include <kernel/raii.hpp>
#include <kernel/spinlock.hpp>
#include <kernel/stream.hpp>

/**
 * %(FLAGS)(WIDTH)(SPECIFIER)
 * 
 * Flags:
 * '-' -> left justify
 * '+' -> show sign
 * ' ' -> show space when positive
 * '0' -> left pad with 0 
 *
 * Specifiers:
 * %s -> string
 * %c -> char
 * %d -> decimal
 * %u -> unsigned
 * %x -> hexadecimal
 * %b -> binary
 * %p -> pointer
 */

namespace detail
{

void advance(OutputStream<char>& out, const char*& format);
void advance(OutputStream<char>& out, const char*& format, void* arg);

inline void dopr(OutputStream<char>& out, const char* format)
{
    advance(out, format);    
}

template<class T>
void dopr(OutputStream<char>& out, const char* format, T arg)
{
    advance(out, format, (void*) (uptr) arg);
    dopr(out, format);
}

template<class T, class... Args>
void dopr(OutputStream<char>& out, const char* format, T arg, Args... args)
{
    advance(out, format, (void*) (uptr) arg);
    dopr(out, format, args...);
}

} // detail


extern OutputStream<char>* printf_out;
extern Spinlock printf_lock;

inline void printf(const char* format)
{
    if (printf_out != nullptr)
    {
	LockGuard guard { printf_lock };
	detail::dopr(*printf_out, format);
    }
}

template<class... Args>
void printf(const char* format, Args... args)
{
    if (printf_out != nullptr)
    {
	LockGuard guard { printf_lock };
	detail::dopr(*printf_out, format, args...);
    }
}
