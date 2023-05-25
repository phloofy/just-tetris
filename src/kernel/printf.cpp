#include <kernel/printf.hpp>
#include <kernel/spinlock.hpp>
#include <kernel/stream.hpp>
#include <kernel/string.hpp>

static constexpr int FS_NONE	= 0;
static constexpr int FS_FLAGS	= 1;
static constexpr int FS_WIDTH	= 2;
static constexpr int FS_PRINT	= 3;

static constexpr int FF_MINUS	= (1 << 0);
static constexpr int FF_PLUS	= (1 << 1);
static constexpr int FF_SPACE	= (1 << 2);
static constexpr int FF_ZERO	= (1 << 3);
static constexpr int FF_SIGNED	= (1 << 4);

static void print_string(OutputStream<char>& out, int flags, int width, const char* value)
{
    if (value == 0)
    {
	value = "<NULL>";
    }

    int len = strlen(value);
    int pad = width - len;
    if (pad < 0)
    {
	pad = 0;
    }
    if (flags & FF_MINUS)
    {
	pad = -pad;
    }

    while (pad > 0)
    {
	out.put(' ');
	pad--;	
    }

    while (*value != 0)
    {
	out.put(*value++);	
    }

    while (pad < 0)
    {
	out.put(' ');
	pad++;
    }
}

static void print_int(OutputStream<char>& out, int flags, int width, int value, int radix)
{
    char sign = 0;    
    unsigned uvalue = value;
    char digits[32];
    int idx = 0;

    if (flags & FF_SIGNED)
    {
	if (value < 0)
	{
	    sign = '-';
	    uvalue = -value;
	}
	else if (flags & FF_PLUS)
	{
	    sign = '+';
	}
	else if (flags & FF_SPACE)
	{
	    sign = ' ';
	}
    }

    do {
	digits[idx++] = "0123456789abcdef"[uvalue % radix];
	uvalue /= radix;
    } while ((uvalue != 0) && (idx < 32));

    int pad = width - idx - (sign ? 1 : 0);
    if (pad < 0)
    {
	pad = 0;
    }
    if (flags & FF_MINUS)
    {
	pad = -pad;
    }

    if (flags & FF_ZERO)
    {
	if (sign != 0)
	{
	    out.put(sign);
	}
	
	while (pad > 0)
	{
	    out.put('0');
	    pad--;
	}
    }
    else
    {	
	while (pad > 0)
	{
	    out.put(' ');
	    pad--;
	}
	
	if (sign != 0)
	{
	    out.put(sign);
	}
    }

    while (idx > 0)
    {
	out.put(digits[--idx]);
    }

    while (pad < 0)
    {
	out.put(' ');
	pad++;
    }
}

namespace detail
{

void advance(OutputStream<char>& out, const char*& format)
{
    char c = *format++;
    while (c != 0)
    {
	out.put(c);
	c = *format++;
    }
}

void advance(OutputStream<char>& out, const char*& format, void* arg)
{   
    int c = *format++;
    int state = FS_NONE;
    int flags = 0;
    int width = 0;
    int digit = 0;

    while (true)
    {
	if (c == 0)
	{
	    return;
	}
	
	switch (state)
	{
	case FS_NONE:
	    if (c == '%')
	    {
		state = FS_FLAGS;
	    }
	    else
	    {
		out.put(c);
	    }
	    c = *format++;
	    break;
	    
	case FS_FLAGS:
	    switch (c)
	    {
	    case '-':
		flags |= FF_MINUS;
		c = *format++;
		break;
	    case '+':
		flags |= FF_PLUS;
		c = *format++;
		break;
	    case ' ':
		flags |= FF_SPACE;
		c = *format++;
		break;
	    case '0':
		flags |= FF_ZERO;
		c = *format++;
		break;
	    default:
		state = FS_WIDTH;
		break;
	    }
	    break;

	case FS_WIDTH:
	    digit = c - '0';
	    if (digit < 10)
	    {
		width = (10 * width) + digit;
		c = *format++;
	    }
	    else
	    {
		state = FS_PRINT;
	    }
	    break;

	case FS_PRINT:
	    switch (c)
	    {
	    case 's':
		print_string(out, flags, width, (const char*) arg);
		break;
	    case 'c':
		print_string(out, flags, width, (const char*) &arg);
		break;
	    case 'd':
		flags |= FF_SIGNED;
	    case 'u':
		print_int(out, flags, width, (int) arg, 10);
		break;
	    case 'p':
	    case 'x':
		print_int(out, flags, width, (int) arg, 16);
		break;
	    case 'b':
		print_int(out, flags, width, (int) arg, 2);
		break;
	    case '%':
		out.put(c);
		break;
	    }
	    return;
	default:
	    return;
	}
    }
}

} // detail

OutputStream<char>* printf_out;
Spinlock printf_lock;
