#include <kernel/acpi.hpp>
#include <kernel/string.hpp>

namespace ACPI
{

const u16* ebda_ptr = (u16*) 0x40e;

RSDP* rsdp = nullptr;

void init()
{
    for (uptr ptr = 0xe0000; ptr < 0x100000; ptr += 16)
    {
	RSDP* tmp = (RSDP*) ptr;
	if (!memcmp(tmp->Signature, "RSD PTR ", 8))
	{
	    rsdp = tmp;
	}
    }

    if (rsdp == nullptr)
    {    
	uptr base = ((uptr) *ebda_ptr) << 4;
	uptr limit = base + (1 << 10);
	for (uptr ptr = base; ptr < limit; ptr += 16)
	{
	    RSDP* tmp = (RSDP*) ptr;
	    if (!memcmp(tmp->Signature, "RSD PTR ", 8))
	    {
		rsdp = tmp;
	    }
	}
    }
}

SDTHeader* sdt(const char* name)
{
    SDTHeader* rsdt = (SDTHeader*) rsdp->RsdtAddress;
    SDTHeader** sdt_ptr = (SDTHeader**) (rsdt + 1);
    u32 count = (rsdt->Length - sizeof(SDTHeader)) >> 2;
    for (; count > 0; count--)
    {
	SDTHeader* sdt = *sdt_ptr++;
	if (!memcmp(sdt->Signature, name, 4))
	{
	    return sdt;
	}
    }
    return nullptr;
}

}
