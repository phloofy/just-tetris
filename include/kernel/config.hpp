#pragma once

#include <kernel/types.hpp>

#define MAX_PROCS 16

struct Config
{
    struct ApicInfo
    {
	u8 ProcessorID;
	u8 APICID;
	u32 Flags;
    } __attribute__ ((packed));

    char manufacturer_id[13];
    u32 features;
    u32 mem_size;
    u32 proc_count;
    u32 madt_flags;
    u32 local_apic;
    u32 io_apic;
    ApicInfo apic_info[MAX_PROCS];
    char oemid[7];
    
    static void init();
};

extern Config kernel_config;
