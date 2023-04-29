#pragma once

template<class T>
constexpr T&& move(T t)
{
    static_cast<T&&>(t);
}

template<class T>
constexpr T&& move(T& t)
{
    static_cast<T&&>(t);
}

template<class T>
constexpr T&& move(T&& t)
{
    static_cast<T&&>(t);
}

template<class T>
void swap(T& lhs, T& rhs)
{
    T tmp = move(lhs);
    lhs = move(rhs);
    rhs = move(tmp);
}
