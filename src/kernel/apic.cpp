#include <kernel/apic.hpp>
#include <kernel/config.hpp>
#include <kernel/idt.hpp>
#include <kernel/machine.hpp>
#include <kernel/types.hpp>

extern "C" void spurious_handler_();
extern "C" void timer_handler_();

static constexpr u32 PIT_FREQ = 1193182;

static constexpr u32 APIC_BASE_MSR = 0x1B;
static constexpr u64 APIC_BASE_ENABLE = 1 << 11;

static constexpr u32 APIC_TIMER_DISABLE = 1 << 16;

static constexpr int TIMER_INTERRUPT_VECTOR = 40;

namespace APIC
{

LocalAPIC* local;
IOAPIC* io;

static u32 counter;

void init()
{
    local = (LocalAPIC*) kernel_config.local_apic;
    io = (IOAPIC*) kernel_config.io_apic;

    IDT::register_interrupt(0xFF, (uptr) spurious_handler_);
}

/**
 * Set APIC timer to generate an IRQ with a frequency of hz
 */
void calibrate_timer(u32 hz)
{
    // disable apic timer
    local->lvt_timer.value = APIC_TIMER_DISABLE;
    local->divide_config.value = 0xB;

    // disable speaker, enable gate
    outb(0x61, (inb(0x61) & ~0x02) | 0x01);

    // square wave generator
    outb(0x43, 0b10110110);

    // set reload register
    u16 divider = (PIT_FREQ / 20) & ~0x1;
    outb(0x42, divider);
    outb(0x42, divider >> 8);

    // start apic timer
    u32 initial = -1;
    local->initial_count.value = initial;

    // wait 20 pit oscillations (1 second)
    for (int i = 0; i < 20; i++)
    {
	while ((inb(0x61) & 0x20) == 0);
	while ((inb(0x61) & 0x20) == 1);
    }

    // disable apic timer
    local->lvt_timer.value = APIC_TIMER_DISABLE;
    u32 ticks = initial - local->current_count.value;

    // disable pit
    outb(0x61, (inb(0x61) & ~0x02) | 0x00);

    counter = ticks / hz;

    IDT::register_interrupt(TIMER_INTERRUPT_VECTOR, (uptr) timer_handler_);
}

void enable()
{
    outb(0x21, 0xFF);
    outb(0xA1, 0xFF);

    wrmsr(0x1B, rdmsr(APIC_BASE_MSR) | APIC_BASE_ENABLE);

    local->spurious_interrupt_vector.value = 0x1FF;
}

} // APIC
