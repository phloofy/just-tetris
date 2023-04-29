#pragma once

template<class T>
class InputStream
{
public:
    virtual T get() = 0;
};

template<class T>
class OutputStream
{
public:
    virtual void put(const T& value) = 0;    
};

template<class T>
class IOStream : public InputStream<T>, public OutputStream<T>
{
};
