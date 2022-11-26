#ifndef _TASKMGR_H_
#define _TASKMGR_H_

#include <libk/arrayqueue.h>
#include <kernel/process.h>

#define MAX_NUM_PROC 7

typedef kernel::ArrayQueue<Process*>* proc_list_t;

struct TaskManager{
    TaskManager();
    void add_process(Process* proc);
    Process* get_process();
    void set_fg_proc(kernel::size_t pid);

    proc_list_t ready_list;
    proc_list_t waiting_list;
    proc_list_t stopped_list;

    kernel::u32 last_proc_id;


private:
    bool is_proc_in_list(Process* proc);
};

#endif
