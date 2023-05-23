#pragma once

#include <kernel/atomic.hpp>
#include <kernel/config.hpp>
#include <kernel/idt.hpp>
#include <kernel/types.hpp>

struct alignas(16) APICRegister : public atomic<u32> {} __attribute__((packed));

struct alignas(16) LocalAPIC
{
    APICRegister reserved1[2];
    APICRegister id;
    APICRegister version;
    APICRegister reserved2[4];
    APICRegister task_priority;
    APICRegister arbitration_priority;
    APICRegister processor_priority;
    APICRegister eoi;
    APICRegister remote_read;
    APICRegister logical_destination;
    APICRegister destination_format;
    APICRegister spurious_interrupt_vector;
    APICRegister in_service[8];
    APICRegister trigger_mode[8];
    APICRegister interrupt_request[8];
    APICRegister error_status;
    APICRegister reserved3[6];
    APICRegister lvt_cmci;
    APICRegister interrupt_command[2];
    APICRegister lvt_timer;
    APICRegister lvt_thermal_sensor;
    APICRegister lvt_performance_monitoring_counters;
    APICRegister lvt_lint0;
    APICRegister lvt_lint1;
    APICRegister lvt_error;
    APICRegister initial_count;
    APICRegister current_count;
    APICRegister reserved4[4];
    APICRegister divide_configuration;
    APICRegister reserved5[1];
} __attribute__((packed));

struct alignas(16) IOAPIC
{
    APICRegister index;
    APICRegister data;
} __attribute__((packed));

namespace SMP
{

extern LocalAPIC* local_apic;
extern IOAPIC* io_apic;

inline uint core()
{
    return local_apic->id.load() >> 24;    
}

inline void eoi()
{
    local_apic->eoi.store(0);
}

inline void ipi(u32 id, u32 num)
{
    local_apic->interrupt_command[1].store(id << 24);
    local_apic->interrupt_command[0].store(num);
    while ((local_apic->interrupt_command[0].load() & (1 << 12)) != 0);
}

void init();
void core_init();

} // SMP

template<class T>
class PerCore
{
    struct alignas(64) CacheLine
    {
	T object;
    };
    
public:

    T& operator[](int index)
    {
	return data[index].object;
    }

    T& operator()()
    {
	return data[SMP::core()].object;
    }
    
private:

    CacheLine data[MAX_PROCS];    
};
