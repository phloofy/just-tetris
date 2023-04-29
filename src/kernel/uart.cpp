#include <kernel/ports.hpp>
#include <kernel/types.hpp>
#include <kernel/uart.hpp>

char UART::get()
{
    while (!(ports[0x3f8 + 5] & 0x01));
    return ports[0x3f8];
}

void UART::put(const char& value)
{
    while (!(ports[0x3f8 + 5] & 0x20));
    ports[0x3f8] = u8(value);
}
