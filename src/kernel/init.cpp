#include <kernel/acpi.hpp>
#include <kernel/barrier.hpp>
#include <kernel/config.hpp>
#include <kernel/crt.hpp>
#include <kernel/idt.hpp>
#include <kernel/mm.hpp>
#include <kernel/printf.hpp>
#include <kernel/smp.hpp>
#include <kernel/task.hpp>
#include <kernel/types.hpp>
#include <kernel/uart.hpp>

struct stack
{    
    static constexpr usize BYTES = 4096;
    byte bytes[BYTES] __attribute__((aligned(16)));
};

static stack stacks[16];
static UART uart;
static atomic<uint> running = 0;
static bool global_init_done = false;

extern "C" void* kernel_stack()
{
    return &stacks[global_init_done ? SMP::core() : 0].bytes[stack::BYTES];
}

static void global_init()
{
    printf_out = &uart;
    printf("hello world\n");

    ACPI::init();    
    Config::init();

    Config& kconf = kernel_config;
    printf("Config:\n");
    printf("\t%s\n", kconf.manufacturer_id);
    printf("\tSSE        : %s\n", (kconf.features & (1 << 0)) ? "true" : "false");
    printf("\tMONITOR    : %s\n", (kconf.features & (1 << 3)) ? "true" : "false");
    printf("\tHypervisor : %s\n", (kconf.features & (1 << 31)) ? "true" : "false");

    printf("\tOEMID      : %s\n", kconf.oemid);
    printf("\tProcessors : %d\n", kconf.proc_count);
    printf("\tMemory     : 0x%08x (%d.%dMB)\n", kconf.mem_size, kconf.mem_size >> 20,
	   ((kconf.mem_size & 0xfffff) * 10) / 0x100000);
    printf("\tLocal APIC : 0x%x\n", kconf.local_apic);
    printf("\tIO APIC    : 0x%x\n", kconf.io_apic);

    kmem_init(0x100000, kconf.mem_size);
    
    CRT::init();
  
    IDT::init();

    task_init();
    
    global_init_done = true;
}


static void per_core_init()
{
    /* TODO
     *
     * enable apit
     *
     * tss initialization
     *
     * enable interrupts
     */
}

extern "C" void smp_entry();
extern void kernel_main();

Barrier* barrier;

extern "C" void kernel_init()
{
    if (!global_init_done)
    {
	global_init();

	Config& kconf = kernel_config;	

	APIC::init();
	APIC::enable();
	
	barrier = new Barrier(kconf.proc_count);
	
	running.add(1);
	
	for (uint id = 1; id < kconf.proc_count; id++)
	{
	    SMP::ipi(id, 0x4500);
	    SMP::ipi(id, (0x4600 | (((uptr) smp_entry) >> 12)));
	    
	    while (running.load() <= id);
	}       

	task([] {
	    kernel_main();
	    while (true)
	    {
		outb(0xF4, 0);
		pause();
	    }
	});
	
	/* TODO
	 * apit calibration
	 *
	 * create main thread
	 */
    }
    else
    {
	running.add(1);
	APIC::enable();
    }
    per_core_init();

    CurrentTask::idle();
}
