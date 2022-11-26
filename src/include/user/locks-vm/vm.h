#pragma once

#include "../../../user/locks-vm/code/code.h"
#include "../../../user/locks-vm/code/code_builder.h"
#include "../../../user/locks-vm/stack/frame.h"
#include "../../../user/locks-vm/instruction.h"
#include "../../../user/locks-vm/mem/vmmem.h"


class VirtualMachine{
    public:
        VirtualMachine(code_array_t* code);
        ~VirtualMachine();

        void run();

    private:
        Code* _code_obj;
        Frame* _cur_frame;
        Frame* _main_frame;
        kernel::Stack<Frame*>* _call_stack;
        kernel::i32 _ip;
        kernel::u8 _cur_ins;
        VMMemoryManager* mem_mgr;

        kernel::size_t _advance(kernel::size_t advance_by = 1);
        void _goto(kernel::size_t idx);
        void _push_frame(Frame* f);
        Frame* _pop_frame();
        void _init_vm();
        bool _is_truthy(LObject* obj);
        void execute(opcode i);
        void _is_not_implemented(kernel::size_t i);
};
