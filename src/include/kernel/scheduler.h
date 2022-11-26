#ifndef _SCHEDULER_H_
#define _SCHEDULER_H_

#include <cpu/timer.h>
#include <kernel/process.h>

struct Scheduler{
    Scheduler();
    void run();
    void pause();
    Process* get_cur_proc();
    void set_cur_proc(Process* p);
    Process* get_fg_proc();
    void set_fg_proc(Process* p);

    void (*timer_callback_ptr)();
    Timer* timer;
};

#endif