#pragma once

#include <kernel/types.hpp>

extern "C" u8 inb(uint port);
extern "C" u16 inw(uint port);
extern "C" u32 inl(uint port);

extern "C" void outb(uint port, u8 number);
extern "C" void outw(uint port, u16 number);
extern "C" void outl(uint port, u32 number);

class Port
{
public:
    Port(uint number);
	
    operator u8() const;
    explicit operator u16() const;
    explicit operator u32() const;
	
    Port& operator=(u8 value);
    Port& operator=(u16 value);
    Port& operator=(u32 value);
	
private:
    uint number;
};

class PortIO
{    
public:    
    Port operator[](usize idx)
    {
	return Port(idx);
    }
};

extern PortIO ports;
