#pragma once

#include <libk/stack.h>
#include "../types.h"

#define MAX_NUM_LOCAL_VARS 5

class Frame{
    public:
        Frame();
        Frame(Frame* f);
        Frame(const char* n);
        ~Frame();

        const char* get_name(){ return this->name; }
        kernel::Stack<LObject*>* get_operand_stack(){ return this->_operand_stack; }
        kernel::Array<LObject*>* get_local_vars(){ return this->_local_vars; }
        kernel::Vector<kernel::u8>* get_code(){ return this->_code; }
        

        void push_opstack(LObject* obj);
        LObject* pop_opstack();

        void set_return_adress(kernel::size_t adr);
        kernel::size_t get_return_adress();

        LObject* get_local_var_at_index(kernel::size_t i);
        void set_local_var_at_index(kernel::size_t i, LObject* obj);
        void set_local_vars(kernel::Array<LObject*>* vars);
        
        void set_code(kernel::Vector<kernel::u8>* code);

        kernel::u8 get_ins_at_index(kernel::size_t i);

        void reset(); // ?????

    private:
        const char* name;
        kernel::Stack<LObject*>* _operand_stack;
        kernel::Array<LObject*>* _local_vars;
        kernel::Vector<kernel::u8>* _code;
        kernel::size_t _ret_address;
};
