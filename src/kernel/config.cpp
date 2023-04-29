#include <kernel/acpi.hpp>
#include <kernel/config.hpp>
#include <kernel/machine.hpp>
#include <kernel/string.hpp>
#include <kernel/types.hpp>

struct MemInfo
{    
    u16 ax;
    u16 bx;
    u16 cx;
    u16 dx;
};

extern MemInfo mem_info;

Config kernel_config;

void Config::init()
{
    using namespace ACPI;
    
    Config& conf = kernel_config;

    cpuid_regs out;
    cpuid(0, &out);
    memcpy(conf.manufacturer_id + 0, &out.ebx, 4);
    memcpy(conf.manufacturer_id + 4, &out.edx, 4);
    memcpy(conf.manufacturer_id + 8, &out.ecx, 4);
    conf.manufacturer_id[12] = 0;

    cpuid(1, &out);
    conf.features = out.ecx;    
    
    conf.mem_size
	= (((u32) (mem_info.ax | mem_info.cx)) << 10)
	+ (((u32) (mem_info.bx | mem_info.dx)) << 16)
	+ 0x100000;

    if (rsdp != nullptr)
    {
	for (int i = 0; i < 6; i++)
	{
	    conf.oemid[i] = rsdp->OEMID[i];
	}
	conf.oemid[6] = 0;
    }

    MADT* madt = (MADT*) sdt("APIC");

    conf.local_apic = madt->LocalInterruptControllerAddress;
    conf.madt_flags = madt->Flags;
    
    isize bytes = madt->Header.Length - sizeof(MADT);
    
    MADTHeader* entry = (MADTHeader*) (madt + 1);
    
    conf.proc_count = 1;
    while (bytes > 0)
    {
	u32 len = entry->Length;
	entry = (MADTHeader*) (((uptr) entry) + len);
	bytes -= len;

	switch (entry->Type)
	{
	case 0:
	{
	    ProcessorLocalAPICStructure* apic = (ProcessorLocalAPICStructure*) entry;
	    if (apic->ACPIProcessorUID != 0)
	    {
		Config::ApicInfo* info = &conf.apic_info[conf.proc_count++ - 1];
		info->ProcessorID = apic->ACPIProcessorUID;
		info->APICID = apic-> APICID;
		info->Flags = apic->Flags;
	    }
	}
	case 1:
	{
	    IOAPICStructure* apic = (IOAPICStructure*) entry;
	    conf.io_apic = apic->IOAPICAddress;
	    break;
	}
	default:
	    break;
	}	
    }
}
