#pragma once

#include <cpu/cpu.h>
#include <device/serial.h>
#include <kernel/scheduler.h>
#include <kernel/vfs.h>
#include <device/iomgr.h>
#include <kernel/taskmgr.h>
#include <device/keyboard.h>
#include <device/sdcard.h>
#include <device/drive.h>
#include <device/esp01.h>

namespace kernel{
    void init();
    void start_sched();

    CPU* get_cpu();
    Serial* get_serial_device();
    Keyboard* get_keyboard();
    SDCard* get_sdcard();
    ESP01* get_wifi_module();
    Drive* get_root_drive();

    Scheduler* get_scheduler();
    TaskManager* get_taskmgr();
    IOManager* get_iomgr();
    VFS* get_vfs();

    long get_clock_ctr();
    void inc_clock_ctr();
    bool sched_started();
    void set_sched_started();
};

