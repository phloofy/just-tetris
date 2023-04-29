#pragma once

#include <kernel/machine.hpp>

template<class T>
class atomic
{
public:
    atomic()		: value() {}    
    atomic(T value)	: value(value) {}

    T load(int memorder = __ATOMIC_SEQ_CST) const
    {
	return __atomic_load_n(&value, memorder);
    }

    void store(T val, int memorder = __ATOMIC_SEQ_CST)
    {
	__atomic_store_n(&value, val, memorder);
    }

    T exchange(T val, int memorder) {
	T ret;
	__atomic_exchange(&value, &val, &ret, memorder);
	return ret;
    }

    bool compare_exchange(T expected, T desired, int memorder = __ATOMIC_SEQ_CST)
    {
	return __atomic_compare_exchange(&value, &expected, &desired, false, memorder, memorder);
    }

    T add_fetch (T val, int memorder = __ATOMIC_SEQ_CST) { return __atomic_add_fetch (&value, val, memorder); }
    T sub_fetch (T val, int memorder = __ATOMIC_SEQ_CST) { return __atomic_sub_fetch (&value, val, memorder); }
    T and_fetch (T val, int memorder = __ATOMIC_SEQ_CST) { return __atomic_and_fetch (&value, val, memorder); }
    T xor_fetch (T val, int memorder = __ATOMIC_SEQ_CST) { return __atomic_xor_fetch (&value, val, memorder); }
    T or_fetch  (T val, int memorder = __ATOMIC_SEQ_CST) { return __atomic_or_fetch  (&value, val, memorder); }
    T nand_fetch(T val, int memorder = __ATOMIC_SEQ_CST) { return __atomic_nand_fetch(&value, val, memorder); }

    T fetch_add (T val, int memorder = __ATOMIC_SEQ_CST) { return __atomic_fetch_add (&value, val, memorder); }
    T fetch_sub (T val, int memorder = __ATOMIC_SEQ_CST) { return __atomic_fetch_sub (&value, val, memorder); }
    T fetch_and (T val, int memorder = __ATOMIC_SEQ_CST) { return __atomic_fetch_and (&value, val, memorder); }
    T fetch_xor (T val, int memorder = __ATOMIC_SEQ_CST) { return __atomic_fetch_xor (&value, val, memorder); }
    T fetch_or  (T val, int memorder = __ATOMIC_SEQ_CST) { return __atomic_fetch_or  (&value, val, memorder); }
    T fetch_nand(T val, int memorder = __ATOMIC_SEQ_CST) { return __atomic_fetch_nand(&value, val, memorder); }

    T add(T val, int memorder = __ATOMIC_SEQ_CST) { return add_fetch(val, memorder); }
    T sub(T val, int memorder = __ATOMIC_SEQ_CST) { return sub_fetch(val, memorder); }
    
    bool test_and_set(int memorder = __ATOMIC_SEQ_CST)
    {
	return __atomic_test_and_set(&value, memorder);
    }

    void clear(int memorder = __ATOMIC_SEQ_CST)
    {
	__atomic_clear(&value, memorder);
    }

    void monitor()
    {
	::monitor(&value);
    }
    
    operator T() const { return load(); }
    atomic<T>& operator=(T val) { return store(val); }
    
    atomic<T>& operator+=(T val) { add_fetch(val); return *this; }
    atomic<T>& operator-=(T val) { sub_fetch(val); return *this; }
    atomic<T>& operator&=(T val) { and_fetch(val); return *this; }
    atomic<T>& operator^=(T val) { xor_fetch(val); return *this; }
    atomic<T>& operator|=(T val) { or_fetch(val);  return *this; }

    atomic<T>& operator++() { add_fetch(1); return *this; }
    atomic<T>& operator--() { sub_fetch(1); return *this; }
    
    atomic<T> operator++(int) { return atomic<T>(fetch_add(1)); }    
    atomic<T> operator--(int) { return atomic<T>(fetch_sub(1)); }

private:
    volatile T value;
};
