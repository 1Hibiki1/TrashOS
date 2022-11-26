#include <kernel/process.h>
#include <libk/mem.h>
#include <kernel/kernel.h>

kernel::size_t last_id = 0;

Process::Process(void (*e)(void))
    :   entry(e)
{
    this->stack = (process_stack_t)kernel::malloc(PROC_STACK_SIZE);

    // r14, r11 - r4
    this->stack[(PROC_STACK_SIZE/4)-1] = 0x01000000;
    this->stack[(PROC_STACK_SIZE/4)-2] = (kernel::u32)entry;
    this->stack[(PROC_STACK_SIZE/4)-3] = 0xfffffff9;
    this->stack[(PROC_STACK_SIZE/4)-4] = 12;
    this->stack[(PROC_STACK_SIZE/4)-5] = 3;
    this->stack[(PROC_STACK_SIZE/4)-6] = 2;
    this->stack[(PROC_STACK_SIZE/4)-7] = 1;
    this->stack[(PROC_STACK_SIZE/4)-8] = 0;

    this->stack[(PROC_STACK_SIZE/4)-9] = 0xfffffff9;
    this->stack[(PROC_STACK_SIZE/4)-10] = 11;
    this->stack[(PROC_STACK_SIZE/4)-11] = 10;
    this->stack[(PROC_STACK_SIZE/4)-12] = 9;
    this->stack[(PROC_STACK_SIZE/4)-13] = 8;
    this->stack[(PROC_STACK_SIZE/4)-14] = 7;
    this->stack[(PROC_STACK_SIZE/4)-15] = 6;
    this->stack[(PROC_STACK_SIZE/4)-16] = 5;
    this->stack[(PROC_STACK_SIZE/4)-17] = 4;

    this->id = last_id++;
    this->state = P_STATE_READY;

    this->cpu_state = (CPU_state*)&this->stack[(PROC_STACK_SIZE/4)-17];
    this->readdir_no = 0;

    this->children = new kernel::Vector<Process*>();
    kernel::strcpy(this->cur_dir, "/");

    this->file_desc_table = new kernel::Vector<file_descriptor*>();

    file_descriptor* stdin = new file_descriptor(kernel::get_vfs()->find_vnode("/dev/kbd"));
    file_descriptor* stdout = new file_descriptor(kernel::get_vfs()->find_vnode("/dev/tty0"));
    file_descriptor* stderr = new file_descriptor(kernel::get_vfs()->find_vnode("/dev/tty0"));

    this->add_filedes(stdin);
    this->add_filedes(stdout);
    this->add_filedes(stderr);
}

Process::Process(Process* p, void (*e)(void))
:   entry(e)
{
    this->stack = (process_stack_t)kernel::malloc(PROC_STACK_SIZE);

    // r14, r11 - r4
    this->stack[(PROC_STACK_SIZE/4)-1] = 0x01000000;
    this->stack[(PROC_STACK_SIZE/4)-2] = (kernel::u32)entry;
    this->stack[(PROC_STACK_SIZE/4)-3] = 0xfffffff9;
    this->stack[(PROC_STACK_SIZE/4)-4] = 12;
    this->stack[(PROC_STACK_SIZE/4)-5] = 3;
    this->stack[(PROC_STACK_SIZE/4)-6] = 2;
    this->stack[(PROC_STACK_SIZE/4)-7] = 1;
    this->stack[(PROC_STACK_SIZE/4)-8] = 0;

    this->stack[(PROC_STACK_SIZE/4)-9] = 0xfffffff9;
    this->stack[(PROC_STACK_SIZE/4)-10] = 11;
    this->stack[(PROC_STACK_SIZE/4)-11] = 10;
    this->stack[(PROC_STACK_SIZE/4)-12] = 9;
    this->stack[(PROC_STACK_SIZE/4)-13] = 8;
    this->stack[(PROC_STACK_SIZE/4)-14] = 7;
    this->stack[(PROC_STACK_SIZE/4)-15] = 6;
    this->stack[(PROC_STACK_SIZE/4)-16] = 5;
    this->stack[(PROC_STACK_SIZE/4)-17] = 4;

    this->id = last_id++;
    this->state = P_STATE_READY;
    this->readdir_no = 0;

    this->cpu_state = (CPU_state*)&this->stack[(PROC_STACK_SIZE/4)-17];
    this->parent = p;
    this->cur_signal = PROC_SIG_NONE;

    kernel::strcpy(cur_dir, p->cur_dir);

    this->children = new kernel::Vector<Process*>();
    kernel::strcpy(this->cur_dir, p->cur_dir);

    this->file_desc_table = new kernel::Vector<file_descriptor*>(p->file_desc_table);
}

Process::~Process(){
    kernel::free(this->stack);
    delete this->children;
    delete this->file_desc_table;
}

void Process::add_filedes(file_descriptor* fd){
    kernel::size_t ndesc = this->file_desc_table->get_size();
    if(ndesc > 0){
        fd->fd_num = this->file_desc_table->get_idx(
            ndesc - 1
        )->fd_num + 1;
    }
    else
        fd->fd_num = 0;
    this->file_desc_table->push(fd);
}

file_descriptor* Process::get_filedes(kernel::size_t fd_no){
    kernel::size_t n_fd = this->file_desc_table->get_size();

    for(kernel::size_t i = 0; i < n_fd; i++){
        file_descriptor* cur = this->file_desc_table->get_idx(i);
        if(cur->fd_num == fd_no)
            return cur;
    }

    return nullptr;
}

file_descriptor* Process::remove_filedes(kernel::size_t fd_no){
    kernel::size_t n_fd = this->file_desc_table->get_size();

    for(kernel::size_t i = 0; i < n_fd; i++){
        file_descriptor* cur = this->file_desc_table->get_idx(i);
        if(cur->fd_num == fd_no){
            this->file_desc_table->delete_idx(i);
            return cur;
        }
    }

    return nullptr;
}
