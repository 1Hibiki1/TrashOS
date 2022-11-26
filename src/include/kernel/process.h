#pragma once

#include <libk/stdint.h>
#include <cpu/cpu.h>
#include <kernel/vnode.h>
#include <libk/vector.h>

#define PROC_STACK_SIZE 0x400   // 1k

class Process;

using process_stack_t = kernel::u32*;
using process_heap_t = void*;
using fd_table = kernel::Vector<file_descriptor*>*;
using process_list = kernel::Vector<Process*>*;

enum PROC_SIGNAL{
    PROC_SIGSTOP,
    PROC_SIGINT,
    PROC_SIG_NONE
};

enum PROC_STATE{
    P_STATE_READY,
    P_STATE_WAITING,
    P_STATE_EXITED,
};

class Process{
    public:
        Process(
            void (*entry)(void)
        );
        Process(Process* p, void (*e)(void));
        ~Process();
        
        void push_inp_buf(kernel::u8 dat);
        void add_filedes(file_descriptor* fd);
        file_descriptor* get_filedes(kernel::size_t fd_no);
        file_descriptor* remove_filedes(kernel::size_t fd_no);

        void set_state(PROC_STATE st){
            this->state = st;
        }

        PROC_STATE get_state(){
            return this->state;
        }

        kernel::size_t get_id(){
            return this->id;
        }

        Process* get_parent(){
            return this->parent;
        }

        void set_parent(Process* p){
            this->parent = p;
        }

        void add_child(Process* p){
            this->children->push(p);
        }

        void remove_child(Process* p){
            kernel::size_t sz = this->children->get_size();

            // TODO: this is done often enough, why not have a find function in vector?
            for(kernel::size_t i = 0; i < sz; i++){
                if(this->children->get_idx(i) == p){
                    this->children->delete_idx(i);
                }
            }
        }

        void set_signal(PROC_SIGNAL sig){
            this->cur_signal = sig;
        }

        void clear_signal(){
            this->cur_signal = PROC_SIG_NONE;
        }

        kernel::size_t get_readdir_no(){
            return this->readdir_no;
        }

        void set_readdir_no(kernel::size_t n){
            this->readdir_no = n;
        }

        char* get_cur_dir(){
            return this->cur_dir;
        }

        void set_cur_dir(const char* d){
            kernel::strncpy(this->cur_dir, d, 20);
        }

        process_list get_children(){
            return this->children;
        }

    private:
        CPU_state* cpu_state;
        kernel::size_t id;
        PROC_STATE state;
        process_stack_t stack;
        void (*entry)(void);
        fd_table file_desc_table;
        process_list children;
        Process* parent;
        PROC_SIGNAL cur_signal; 
        kernel::size_t readdir_no;
        char cur_dir[20];
};
