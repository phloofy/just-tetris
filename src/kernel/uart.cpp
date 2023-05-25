#include <kernel/machine.hpp>
#include <kernel/types.hpp>
#include <kernel/uart.hpp>

static constexpr int COM1 = 0x3F8;
static constexpr int LINE_STATUS_OFFSET = 5;

static constexpr u8 LINE_STATUS_DR = 1 << 0;
static constexpr u8 LINE_STATUS_THRE = 1 << 5;

char UART::get()
{
    while (!(inb(COM1 + LINE_STATUS_OFFSET) & LINE_STATUS_DR));
    return inb(COM1);
}

void UART::put(const char& value)
{
    while (!(inb(COM1 + LINE_STATUS_OFFSET) & LINE_STATUS_THRE));
    outb(COM1, value);
}
