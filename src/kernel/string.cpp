#include <kernel/string.hpp>

void* memcpy(void* target, const void* source, usize size)
{
    byte* tgt = (byte*) target;
    const byte* src = (const byte*) source;
    for (; size != 0; size--)
    {
	*tgt++ = *src++;
    }
    return target;
}

int memcmp(const void* ptr1, const void* ptr2, usize size)
{
    const byte* p1 = (const byte*) ptr1;
    const byte* p2 = (const byte*) ptr2;
    for (; size != 0; size--)
    {
	byte c1 = *p1++;
	byte c2 = *p2++;
	if (c1 != c2)
	{
	    return c1 - c2;
	}
    }
    return 0;
}

void* memset(void* target, byte value, usize size)
{
    byte* tgt = (byte*) target;
    for (; size != 0; size--)
    {
	*tgt++ = value;
    }
    return target;
}

int strcmp(const char* str1, const char* str2)
{
    while (true)
    {
	char c1 = *str1++;
	char c2 = *str2++;
	if (c1 != c2)
	{
	    return c1 - c2;
	}
	else if (c1 == 0)
	{
	    return 0;
	}
    }
}

const char* strchr(const char* str, char chr)
{
    while (*str++ != chr);
    return str;
}

usize strlen(const char* str)
{
    return strchr(str, 0) - str;
}
