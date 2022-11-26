#include <kernel/scheduler.h>
#include <kernel/kernel.h>
#include <cpu/tm4c.h>
#include <kernel/process.h>
#include <libk/array.h>
#include <kernel/taskmgr.h>
#include <stddef.h>

bool sw = false;
Process* cur_pcb = nullptr;
Process* fg_pcb = nullptr;

extern "C" void scheduler_get_next_task(){
    Process* next_p = kernel::get_taskmgr()->get_process();

    if(cur_pcb != nullptr && cur_pcb->get_state() == P_STATE_READY)
        kernel::get_taskmgr()->add_process(cur_pcb);

    cur_pcb = next_p;
}


extern "C" __attribute__((naked)) void PendSV_Handler(){
    __asm volatile (
        "cpsid if                       \n"
        "push {r4-r11, r14}             \n"

        "ldr r2, =cur_pcb               \n"
        "ldr r3, [r2]                   \n"

        // r3 contains the address of the current pcb

        // check if first task
        "cmp r3, #0                     \n"
        "bne not_first_task             \n"

        "push {r0-r3, r12}              \n"
        "bl scheduler_get_next_task     \n" //get first pcb
        "pop {r0-r3, r12}               \n"

        "ldr r3, [r2]                   \n"
        "b first_task                   \n"

        // else
        "not_first_task:                \n"
        "str sp, [r3]                   \n" // save context

        "push {r0-r3, r12}              \n"
        "bl scheduler_get_next_task     \n" // get next pcb
        "pop {r0-r3, r12}               \n"

        "ldr r3, [r2]                   \n"
        
        "first_task:                    \n"
        "ldr sp, [r3]                   \n" //load context
        "pop {r4-r11, r14}              \n"
        "cpsie if                       \n"
        "bx lr                          \n" //return
    );
}


void timer_callback(){
    kernel::get_cpu()->disable_interrupts();
    kernel::inc_clock_ctr();

    if(sw){
        kernel::get_cpu()->portF->write_pin(2, 1);
        sw = false;
    }

    else{
        kernel::get_cpu()->portF->write_pin(2, 0);
        sw = true;
    }

    kernel::get_iomgr()->process_requests();

    kernel::get_cpu()->enable_interrupts();

    if(kernel::get_taskmgr()->ready_list->get_size() > 0)
        //TODO: move this to cpu?
        NVIC_INT_CTRL_R = NVIC_INT_CTRL_PEND_SV;

}

Scheduler::Scheduler(){
    this->timer_callback_ptr = timer_callback;
    this->timer = new Timer(TIMER_SYSTICK, 8000);
    this->timer->set_callback(this->timer_callback_ptr);
}

void Scheduler::run(){
    NVIC_ST_CTRL_R = 0x00000007;
}

void Scheduler::pause(){
    NVIC_ST_CTRL_R = 0;
}

Process* Scheduler::get_cur_proc(){
    return cur_pcb;
}

void Scheduler::set_cur_proc(Process* p){
    cur_pcb = p;
}

Process* Scheduler::get_fg_proc(){
    return fg_pcb;
}

void Scheduler::set_fg_proc(Process* p){
    fg_pcb = p;
}
