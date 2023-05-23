#include <kernel/idt.hpp>
#include <kernel/ports.hpp>
#include <kernel/smp.hpp>
#include <kernel/types.hpp>

namespace SMP
{

extern "C" u64 rdmsr(u32 id);
extern "C" void wrmsr(u32 id, u64 val);
extern "C" void spurious_handler_();

LocalAPIC* local_apic;
IOAPIC* io_apic;

void init()
{
    IDT::register_interrupt(0xFF, (uptr) spurious_handler_);
}

void core_init()
{
    ports[0x21] = u8(0xFF);
    ports[0xA1] = u8(0xFF);

    u64 msr = rdmsr(0x1B);
    wrmsr(0x1B, msr | (1 << 1));

    local_apic->spurious_interrupt_vector.store(0x1FF);
}

}
