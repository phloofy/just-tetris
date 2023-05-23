#include <kernel/printf.hpp>
#include <kernel/smp.hpp>
#include <kernel/types.hpp>

void kernel_main()
{
    printf("0x%x\n", &((LocalAPIC*) 0)->divide_configuration);
    
}
