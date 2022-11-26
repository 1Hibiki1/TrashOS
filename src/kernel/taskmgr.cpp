#include <kernel/taskmgr.h>
#include <kernel/debug.h>
#include <kernel/kernel.h>


bool TaskManager::is_proc_in_list(Process* proc){
    for(kernel::size_t i = 0; i < this->ready_list->get_size(); i++){
        if(this->ready_list->get_idx(i)->get_id() == proc->get_id()) return true;
    }

    return false;
}


TaskManager::TaskManager(){
    this->ready_list = new kernel::ArrayQueue<Process*>(MAX_NUM_PROC);
    this->waiting_list = new kernel::ArrayQueue<Process*>(MAX_NUM_PROC);
    this->stopped_list = new kernel::ArrayQueue<Process*>(MAX_NUM_PROC);

    this->last_proc_id = 0;
}

void TaskManager::add_process(Process* proc){
    REQUIRES(!this->is_proc_in_list(proc));
    REQUIRES(proc->get_state() == P_STATE_READY);
    this->ready_list->enqueue(proc);
}

Process* TaskManager::get_process(){
    Process* p = this->ready_list->dequeue();

    ENSURES(p->get_state() == P_STATE_READY);
    return p;
}

void TaskManager::set_fg_proc(kernel::size_t pid){
    kernel::get_scheduler()->pause();
    if(kernel::get_scheduler()->get_cur_proc()->get_id() == pid){
        kernel::get_scheduler()->set_fg_proc(kernel::get_scheduler()->get_cur_proc());
        kernel::get_scheduler()->run();
        return;
    }
    for(kernel::size_t i = 0; i < this->ready_list->get_size(); i++){
        if(this->ready_list->get_idx(i)->get_id() == pid){
            kernel::get_scheduler()->set_fg_proc(this->ready_list->get_idx(i));
            kernel::get_scheduler()->run();
            return;
        }
    }
    kernel::get_scheduler()->run();
}
