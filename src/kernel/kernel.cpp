#include <kernel/kernel.h>
#include <kernel/vfs.h>
#include <kernel/vnode.h>

#include <libk/mem.h>
#include <libk/stdio.h>

#include <cpu/interrupts.h>
#include <cpu/cpu.h>

#include <device/serial.h>
#include <device/iomgr.h>
#include <device/keyboard.h>
#include <device/sdcard.h>
#include <device/ramdisk.h>
#include <device/drive.h>
#include <device/esp01.h>

#include <cstdarg>

#include "umm_malloc/umm_malloc.h"

const bool use_SDCard = true;
const bool use_ramdisk = false;
const bool use_kbd = true;
const bool use_serial = true;
const bool use_wifi = true;

CPU* cpu;
Serial* serial;
Keyboard* keyboard;
SDCard* sdcard;
RamDisk* ramdisk;
ESP01* wifi_module;

Drive* rootdrive;
Drive* sddrive;
Drive* ramdrive;

Scheduler* scheduler;
TaskManager* taskmgr;
IOManager* iomgr;
VFS* vfs;

bool has_sched_started = false;
long clk_ticks;

void init_devices(){
    if(use_serial){
        serial = new Serial(80, 25);
        serial->clrscr();

        File* tty_file = new File(
            "/dev/tty0", 0, false, sdcard->get_out_buf()->get_data_ptr(), -1
        );
        
        vnode* tty_vnode = new vnode(tty_file, FILE_TYPE_DEVICE);
        vfs->add_vnode(tty_vnode);
    }

    if(use_SDCard){
        kernel::log_info("initializing sd card...\n");
        sdcard = new SDCard();
        sddrive = new Drive(sdcard);
        rootdrive = sddrive;

        File* sd_file = new File(
            "/dev/sd0", 0, false, sdcard->get_inp_buf()->get_data_ptr(), -1
        );
        
        vnode* sd_vnode = new vnode(sd_file, FILE_TYPE_DEVICE);
        vfs->add_vnode(sd_vnode);
    }

    if(use_ramdisk){
        kernel::log_info("initializing ramdisk (6k)...\n");
        ramdisk = new RamDisk(6*1024);
        ramdrive = new Drive(ramdisk);
        rootdrive = ramdrive;

        File* rd_file = new File(
            "/dev/rd0", 0, false, ramdisk->get_inp_buf()->get_data_ptr(), -1
        );
        
        vnode* rd_vnode = new vnode(rd_file, FILE_TYPE_DEVICE);
        vfs->add_vnode(rd_vnode);
        kernel::log_info("done\n");
    }
    
    if(use_kbd){
        kernel::log_info("initializing keyboard... ");
        keyboard = new Keyboard();

        File* kbd_file = new File(
            "/dev/kbd", 0, false, keyboard->get_inp_buf()->get_data_ptr(), -1
        );
        
        vnode* kbd_vnode = new vnode(kbd_file, FILE_TYPE_DEVICE);
        vfs->add_vnode(kbd_vnode);
        kernel::log_info("done\n");
    }

    if(use_wifi){
        kernel::log_info("initializing wifi...\n");
        wifi_module = new ESP01();

        wifi_module->send_command("AT\r\n");
        wifi_module->wait_for_response();

        wifi_module->send_command("AT+CWMODE=1\r\n");
        wifi_module->wait_for_response();

        kernel::log_info("connecting...\n");
        wifi_module->send_command("AT+CWJAP=\"cs4qatar\",\"csevent16\"\r\n");
        wifi_module->wait_for_response();

        wifi_module->send_command("AT+CIFSR\r\n");
        wifi_module->wait_for_response();

        wifi_module->send_command("AT+CIPMUX=1\r\n");
        wifi_module->wait_for_response();

        kernel::log_info("starting server...\n");
        wifi_module->send_command("AT+CIPSERVER=1,1234\r\n");
        wifi_module->wait_for_response();
        kernel::log_info("done\n");
    }
}

extern int _ebss;

void kernel::init(){
    // kernel::mem_init();
    clk_ticks = 0;
    umm_init_heap(&_ebss, 28000);
    cpu = new CPU();

    taskmgr = new TaskManager();
    iomgr = new IOManager();
    scheduler = new Scheduler();
    vfs = new VFS();

    init_devices();

    kernel::log_info("\nkernel initialization complete\n");
    kernel::log_info("starting shell...\n");
}

void kernel::start_sched(){
    has_sched_started = true;
    scheduler->run();
}

CPU* kernel::get_cpu(){
    return cpu;
}

Serial* kernel::get_serial_device(){
    return serial;
}

Scheduler* kernel::get_scheduler(){
    return scheduler;
}

TaskManager* kernel::get_taskmgr(){
    return taskmgr;
}

IOManager* kernel::get_iomgr(){
    return iomgr;
}

Keyboard* kernel::get_keyboard(){
    return keyboard;
}

SDCard* kernel::get_sdcard(){
    return sdcard;
}

Drive* kernel::get_root_drive(){
    return rootdrive;
}

VFS* kernel::get_vfs(){
    return vfs;
}

long kernel::get_clock_ctr(){
    return clk_ticks;
}

void kernel::inc_clock_ctr(){
    clk_ticks++;
}

bool kernel::sched_started(){
    return has_sched_started;
}

void kernel::set_sched_started(){
    has_sched_started = true;
}

ESP01* kernel::get_wifi_module(){
    return wifi_module;
}
