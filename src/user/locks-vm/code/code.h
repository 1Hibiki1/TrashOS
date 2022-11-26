#pragma once

#include <libk/vector.h>

#define magic_number 0x4d69686f

enum Tag {
    CONSTANT_Integer = 0x3,
    CONSTANT_Double = 0x6,
    CONSTANT_String = 0x8
};


struct func_info{
    func_info(){
        this->argc = 0;
        this->code = new kernel::Vector<kernel::u8>();
    }

    kernel::size_t argc;
    kernel::Vector<kernel::u8>* code;

    ~func_info(){}
};


struct cp_info{
    cp_info(Tag t, void* info)
    :   tag(t),
        info(info)
    {}

    Tag tag;
    void* info;

    ~cp_info(){}
};

class Code{
    public:
        Code(){
            this->const_pool = new kernel::Vector<cp_info*>;
            this->func_pool = new kernel::Vector<func_info*>;
        }

        ~Code(){}

        kernel::Vector<cp_info*>* get_cp(){
            return this->const_pool;
        }

        kernel::Vector<func_info*>* get_fp(){
            return this->func_pool;
        }

        void addToCP(cp_info* ci){
            this->const_pool->push(ci);
        }

        cp_info* getFromCP(kernel::size_t idx){
            return this->const_pool->get_idx(idx);
        }

        void addToFP(func_info* fi){
            this->func_pool->push(fi);
        }

        func_info* getFromFP(kernel::size_t idx){
            return this->func_pool->get_idx(idx);
        }

    private:
        kernel::Vector<cp_info*>* const_pool;
        kernel::Vector<func_info*>* func_pool;
};
