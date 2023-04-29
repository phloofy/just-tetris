#include <kernel/ports.hpp>

Port::Port(uint number) : number(number)
{
}

Port::operator u8() const
{
    return inb(number);
}

Port::operator u16() const
{
    return inw(number);
}

Port::operator u32() const
{
    return inl(number);
}

Port& Port::operator=(u8 value)
{
    outb(number, value);
    return *this;
}

Port& Port::operator=(u16 value)
{
    outw(number, value);
    return *this;
}

Port& Port::operator=(u32 value)
{
    outl(number, value);
    return *this;
}

PortIO port;

