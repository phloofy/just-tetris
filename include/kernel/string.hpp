#pragma once

#include <kernel/types.hpp>

extern void* memcpy(void* target, const void* source, usize size);
extern int memcmp(const void* ptr1, const void* ptr2, usize size);
extern void* memset(void* target, byte value, usize size);

extern int strcmp(const char* str1, const char* str2);
extern const char* strchr(const char* str, char chr);
extern usize strlen(const char* str);
