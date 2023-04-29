#include <kernel/acpi.hpp>
#include <kernel/config.hpp>
#include <kernel/ports.hpp>
#include <kernel/printf.hpp>
#include <kernel/types.hpp>
#include <kernel/uart.hpp>

struct stack
{
    static constexpr u32 BYTES = 4096;
    byte bytes[BYTES] __attribute__((aligned(16)));
};

static stack stacks[16];
static UART uart;
static atomic<uint> running = 0;

extern "C" void* kernel_stack()
{
    return &stacks[0].bytes[stack::BYTES];    
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

    /* TODO
     *
     * memory initialization
     * - memory pools for
     *   - tasks
     *   - pages
     *
     * - kernel heap
     *
     * global constructors
     *
     * thread initialization
     */
    
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


extern "C" void kernel_init()
{
    if (running.fetch_add(1) == 0)
    {
	global_init();

	/* TODO
	 *
	 * smp initialization
	 *
	 * apit calibration
	 *
	 * start other cores
	 *
	 * create main thread
	 */
    }
    per_core_init();

    /* TODO
     *
     * enter idle state
     */
    
    // quick and dirty shutdown
    while (true)
    {
	ports[0xf4] = u8(0x00);
	pause();
    }
}
