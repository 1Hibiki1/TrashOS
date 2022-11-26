#include <kernel/kernel.h>
#include <user/process/init.h>
#include "kernel/initial_disk.h"


const bool format_sd = true;

extern "C" void kmain(){
    kernel::init();
    
    kernel::get_cpu()->portF->digital_init_pin(0, INPUT);
    kernel::get_cpu()->portF->digital_init_pin(1, OUTPUT);
    kernel::get_cpu()->portF->digital_init_pin(2, OUTPUT);
    kernel::get_cpu()->portF->digital_init_pin(3, OUTPUT);
    kernel::get_cpu()->portF->digital_init_pin(4, INPUT);

    kernel::get_cpu()->portF->write_pin(1, 1);

    Process* init = new Process(init_main);

    kernel::get_taskmgr()->add_process(init);
    kernel::get_scheduler()->set_fg_proc(init);

    if(format_sd)
        init_disk();

    kernel::start_sched();

    while(1){
        __asm("wfi  \n");
    }
}
