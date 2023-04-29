#pragma once

#include <kernel/types.hpp>

namespace ACPI
{

struct RSDP
{
    char	Signature[8];
    u8		Checksum;
    char	OEMID[6];
    u8		Revision;
    u32		RsdtAddress;
    u32		Length;
    u64		XsdtAddress;
    u8		ExtendedChecksum;
    u8		reserved[3];
} __attribute__ ((packed));

struct SDTHeader
{
    char	Signature[4];
    u32		Length;
    u8		Revision;
    u8		Checksum;
    char	OEMID[6];
    u64		OEMTableID;
    u32		OEMRevision;
    u32		CreatorID;
    u32		CreatorRevision;    
} __attribute__ ((packed));

struct MADT
{
    SDTHeader	Header;
    u32		LocalInterruptControllerAddress;
    u32		Flags;
} __attribute__ ((packed));

struct MADTHeader
{
    u8		Type;
    u8		Length;
} __attribute__ ((packed));

struct ProcessorLocalAPICStructure
{
    MADTHeader	Header;
    u8		ACPIProcessorUID;
    u8		APICID;
    u32		Flags;
} __attribute__ ((packed));

struct IOAPICStructure
{
    MADTHeader	Header;
    u8		IOAPICID;
    u8		reserved[1];
    u32		IOAPICAddress;
    u32		GlobalSystemInterruptBase;    
} __attribute__ ((packed));

extern RSDP* rsdp;

extern void init();
extern SDTHeader* sdt(const char* name);

}
