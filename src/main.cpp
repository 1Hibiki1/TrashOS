#include <cpu/port.h>
#include <cpu/tm4c.h>
#include <cpu/interrupts.h>
#include <libk/mem.h>
#include <libk/array.h>
#include <device/serial.h>
#include <cpu/timer.h>
#include <libk/stdio.h>
#include <kernel/kernel.h>
#include <kernel/process.h>
#include <libk/arrayqueue.h>
#include <device/request.h>
#include <cstdint>
#include <device/ramdisk.h>
#include <device/sdcard.h>
#include <libk/vector.h>
#include <fs/file.h>
#include <fs/directory.h>
#include <fs/fs.h>
#include <fs/stfs.h>
#include <device/keyboard.h>
#include <libk/string.h>
#include <device/drive.h>
#include <kernel/syscall.h>
#include <device/esp01.h>

#include <user/locks-vm/vm.h>
#include <user/stdio.h>
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
