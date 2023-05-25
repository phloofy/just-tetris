#pragma once

#include <kernel/atomic.hpp>
#include <kernel/types.hpp>

namespace APIC
{

struct alignas(16) Register
{
    atomic<u32> value;
} __attribute__((packed));

struct alignas(16) LocalAPIC
{
    Register reserved1[2];
    Register id;
    Register version;
    Register reserved2[4];
    Register task_priority;
    Register arbitration_priority;
    Register processor_priority;
    Register eoi;
    Register remote_read;
    Register logical_destination;
    Register destination_format;
    Register spurious_interrupt_vector;
    Register in_service[8];
    Register trigger_mode[8];
    Register interrupt_request[8];
    Register error_status;
    Register reserved3[6];
    Register lvt_cmci;
    Register interrupt_command[2];
    Register lvt_timer;
    Register lvt_thermal_sensor;
    Register lvt_performance_monitoring_counters;
    Register lvt_lint0;
    Register lvt_lint1;
    Register lvt_error;
    Register initial_count;
    Register current_count;
    Register reserved4[4];
    Register divide_config;
    Register reserved5[1];
} __attribute__((packed));

struct alignas(16) IOAPIC
{
    Register index;
    Register data;
};

extern LocalAPIC* local;
extern IOAPIC* io;

extern void init();
extern void calibrate_timer(u32 hz);
extern void enable();

} // APIC
