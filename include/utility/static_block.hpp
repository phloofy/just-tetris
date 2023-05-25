#pragma once

#define CONCAT(str1, str2) str1##str2
#define EXPCAT(str1, str2) CONCAT(str1, str2)
#define UNQ_ID(prefix) EXPCAT(prefix, __COUNTER__)

#define STATIC_BLOCK_IMPL1(prefix) STATIC_BLOCK_IMPL2(CONCAT(prefix, _fn), CONCAT(prefix, _var))
#define STATIC_BLOCK_IMPL2(fun, var) \
    static void fun();		     \
    [[maybe_unused]]		     \
    static int var = (fun(), 0);     \
    static void fun()

#define static_block STATIC_BLOCK_IMPL1(UNQ_ID(_static_block_))
