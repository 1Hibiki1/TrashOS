#include <user/locks-vm/vm.h>
#include "instruction.h"
#include "lstdlib.h"
#include <libk/string.h>
#include <string.h>

VirtualMachine::VirtualMachine(code_array_t* code){
    CodeBuilder* cb = new CodeBuilder(code);
    this->_code_obj = cb->get_code_obj();
    delete cb;

    this->_cur_frame = new Frame();
    this->_main_frame = new Frame("main");
    this->_call_stack = new kernel::Stack<Frame*>();

    this->mem_mgr = new VMMemoryManager();

    this->mem_mgr->add_to_delete_at_end(this->_code_obj);
    this->mem_mgr->add_vector_to_delete_at_end<cp_info *>(this->_code_obj->get_cp());
    this->mem_mgr->add_vector_to_delete_at_end<func_info *>(this->_code_obj->get_fp());

    for(kernel::size_t i = 0; i < this->_code_obj->get_fp()->get_size(); i++){
        this->mem_mgr->add_to_delete_at_end(this->_code_obj->get_fp()->get_idx(i));
    }

    this->mem_mgr->add_frame_to_delete_at_end(this->_cur_frame);
    this->mem_mgr->add_stack_to_delete_at_end<Frame *>(this->_call_stack);
    this->mem_mgr->add_frame_to_delete_at_end(this->_main_frame);

    this->_ip = -1;
    this->_cur_ins = 0;
}

VirtualMachine::~VirtualMachine(){
    this->mem_mgr->nuke_useless_objects();
    this->mem_mgr->nuke_garbage(this->_call_stack);
    delete this->mem_mgr;
}

void VirtualMachine::run(){
    this->_init_vm();

    while(this->_cur_ins != END){
        opcode i = (opcode)this->_cur_ins;
        this->execute((opcode)i);
        if(
            i != GOTO &&
            i != POP_JMP_IF_TRUE &&
            i != POP_JMP_IF_FALSE
        ){
            this->_advance();
        }
    }
}

kernel::size_t VirtualMachine::_advance(kernel::size_t advance_by){
    this->_ip += advance_by;
    this->_cur_ins = this->_cur_frame->get_ins_at_index(this->_ip);
    return this->_cur_ins;
}

void VirtualMachine::_goto(kernel::size_t idx){
    this->_ip = idx;
    this->_cur_ins = this->_cur_frame->get_ins_at_index(this->_ip);
}

void VirtualMachine::_push_frame(Frame* f){
    this->_call_stack->push(f);
}

Frame* VirtualMachine::_pop_frame(){
    return this->_call_stack->pop();
}

void VirtualMachine::_init_vm(){
    func_info* main = this->_code_obj->getFromFP(0);
    this->_main_frame->set_code(main->code);
    this->_cur_frame = this->_main_frame;
    this->_advance();
}

bool VirtualMachine::_is_truthy(LObject* obj){
    LObjectType type = obj->get_type();

    switch(type){
        case LTYPE_NUMBER:{
            if(obj->get_val().VAL_INT == 0){
                return false;
            }
            return true;
        }

        case LTYPE_STRING:{
            if(kernel::strlen(obj->get_val().VAL_STR) == 0){
                return false;
            }
            return true;
        }

        case LTYPE_NIL:{
            return false;
        }

        case LTYPE_BOOLEAN:{
            return obj->get_val().VAL_BOOL;
        }

        case LTYPE_ARRAY:{
            if(obj->get_val().VAL_ARRAY->get_size() == 0){
                return false;
            }
            return true;
        }

        default:
            return true;
    }
}

void VirtualMachine::_is_not_implemented(kernel::size_t i){

}


/*
 TODO: have a setval function in LOBject instead of
 popping and pushing a new object to stack every time?
*/
void VirtualMachine::execute(opcode i){
    switch(i){
        case LOAD_NIL: {
            // LNil* n = new LNil();
            LNil* n = (LNil*)this->mem_mgr->allocate_object(LTYPE_NIL);
            this->_cur_frame->push_opstack(n);
            break;
        }

        case LOAD_TRUE: {
            // LBoolean* b = new LBoolean(true);
            LBoolean* b = (LBoolean*)this->mem_mgr->allocate_object(LTYPE_BOOLEAN);
            bool val = true;
            b->set_val(&val);

            this->_cur_frame->push_opstack(b);
            break;
        }

        case LOAD_FALSE: {
            // LBoolean* b = new LBoolean(false);
            LBoolean* b = (LBoolean*)this->mem_mgr->allocate_object(LTYPE_BOOLEAN);
            bool val = false;
            b->set_val(&val);

            this->_cur_frame->push_opstack(b);
            break;
        }

        case LOAD_CONST: {
            kernel::size_t idx = (this->_advance() << 8) + this->_advance();
            cp_info* const_obj = this->_code_obj->getFromCP(idx);

            switch(const_obj->tag){
                case CONSTANT_String: {
                    // LString* str = new LString((char*)const_obj->info);
                    LString* str = (LString*)this->mem_mgr->allocate_object(LTYPE_STRING);
                    str->set_val((char*)const_obj->info);

                    this->_cur_frame->push_opstack(str);
                    break;
                }

                case CONSTANT_Integer: {
                    // LNumber* num = new LNumber(*((int*)const_obj->info));
                    LNumber* num = (LNumber*)this->mem_mgr->allocate_object(LTYPE_NUMBER);
                    num->set_val(const_obj->info);

                    this->_cur_frame->push_opstack(num);
                    break;
                }

                case CONSTANT_Double: {
                    // TODO: handle error
                    break;
                }

                default:
                    break;
            }
            break;
        }

        case BINARY_ADD: {
            LObject* r = this->_cur_frame->pop_opstack();
            LObject* l = this->_cur_frame->pop_opstack();

            LObjectType type = l->get_type();

            if(type == LTYPE_STRING){
                if(r->get_type() != LTYPE_STRING){
                    // TODO: handle type error;
                }

                char* lval = l->get_val().VAL_STR;
                char* rval = r->get_val().VAL_STR;

                size_t total_len = strlen(lval) + strlen(rval) + 1;

                char* newstr = (char*)kernel::malloc(total_len);

                kernel::strcpy(newstr, lval);
                strcat(newstr, rval);

                LString* newstr_obj = new LString((const char*)newstr);

                this->mem_mgr->add_to_delete_at_end(newstr);
                this->mem_mgr->add_object(newstr_obj);
                
                this->_cur_frame->push_opstack(newstr_obj);
            }

            else if(type == LTYPE_NUMBER){
                // TODO: handle type error;
                // LNumber* res = new LNumber(r->get_val().VAL_INT + l->get_val().VAL_INT);
                LNumber* res = (LNumber*)this->mem_mgr->allocate_object(LTYPE_NUMBER);
                int val = r->get_val().VAL_INT + l->get_val().VAL_INT;
                res->set_val(&val);
                
                this->_cur_frame->push_opstack(res);
            }
            break;
        }

        case BINARY_SUBTRACT: {
            LObject* r = this->_cur_frame->pop_opstack();
            LObject* l = this->_cur_frame->pop_opstack();

            LObjectType type = l->get_type();

            if(type != LTYPE_NUMBER){
                // TODO: handle type error
            }

            if(type == LTYPE_NUMBER){
                // LNumber* res = new LNumber(l->get_val().VAL_INT - r->get_val().VAL_INT);
                LNumber* res = (LNumber*)this->mem_mgr->allocate_object(LTYPE_NUMBER);
                int val = l->get_val().VAL_INT - r->get_val().VAL_INT;
                res->set_val(&val);

                this->_cur_frame->push_opstack(res);
            }
            break;
        }

        case BINARY_MULTIPLY: {
            LObject* r = this->_cur_frame->pop_opstack();
            LObject* l = this->_cur_frame->pop_opstack();

            LObjectType type = l->get_type();

            if(type != LTYPE_NUMBER){
                // TODO: handle type error
            }

            if(type == LTYPE_NUMBER){
                // TODO: handle type error;
                // LNumber* res = new LNumber(l->get_val().VAL_INT * r->get_val().VAL_INT);
                LNumber* res = (LNumber*)this->mem_mgr->allocate_object(LTYPE_NUMBER);
                int val = l->get_val().VAL_INT * r->get_val().VAL_INT;
                res->set_val(&val);
                
                this->_cur_frame->push_opstack(res);
            }
            break;
        }

        case BINARY_DIVIDE: {
            LObject* r = this->_cur_frame->pop_opstack();
            LObject* l = this->_cur_frame->pop_opstack();

            LObjectType type = l->get_type();

            if(type != LTYPE_NUMBER){
                // TODO: handle type error
            }

            if(type == LTYPE_NUMBER){
                // TODO: handle type error;
                // LNumber* res = new LNumber(l->get_val().VAL_INT / r->get_val().VAL_INT);
                LNumber* res = (LNumber*)this->mem_mgr->allocate_object(LTYPE_NUMBER);
                int val = l->get_val().VAL_INT / r->get_val().VAL_INT;
                res->set_val(&val);
                
                this->_cur_frame->push_opstack(res);
            }
            break;
        }

        case BINARY_MODULO: {
            LObject* r = this->_cur_frame->pop_opstack();
            LObject* l = this->_cur_frame->pop_opstack();

            LObjectType type = l->get_type();

            if(type != LTYPE_NUMBER){
                // TODO: handle type error
            }

            if(type == LTYPE_NUMBER){
                // TODO: handle type error;
                LNumber* res = (LNumber*)this->mem_mgr->allocate_object(LTYPE_NUMBER);
                int val = l->get_val().VAL_INT % r->get_val().VAL_INT;
                res->set_val(&val);

                this->_cur_frame->push_opstack(res);
            }
            break;
        }

        case BINARY_AND: {
            LObject* r = this->_cur_frame->pop_opstack();
            LObject* l = this->_cur_frame->pop_opstack();

            if(!this->_is_truthy(l)){
                LBoolean* b = (LBoolean*)this->mem_mgr->allocate_object(LTYPE_BOOLEAN);
                bool val = false;
                b->set_val(&val);
                this->_cur_frame->push_opstack(b);
            }

            else if(!this->_is_truthy(r)){
                LBoolean* b = (LBoolean*)this->mem_mgr->allocate_object(LTYPE_BOOLEAN);
                bool val = false;
                b->set_val(&val);
                this->_cur_frame->push_opstack(b);
            }

            else {
                LBoolean* b = (LBoolean*)this->mem_mgr->allocate_object(LTYPE_BOOLEAN);
                bool val = true;
                b->set_val(&val);
                this->_cur_frame->push_opstack(b);
            }

            break;
        }

        case BINARY_OR: {
            LObject* r = this->_cur_frame->pop_opstack();
            LObject* l = this->_cur_frame->pop_opstack();

            if(this->_is_truthy(l)){
                // LBoolean* b = new LBoolean(true);
                LBoolean* b = (LBoolean*)this->mem_mgr->allocate_object(LTYPE_BOOLEAN);
                bool val = true;
                b->set_val(&val);
                this->_cur_frame->push_opstack(b);
            }

            else if(this->_is_truthy(r)){
                // LBoolean* b = new LBoolean(true);
                LBoolean* b = (LBoolean*)this->mem_mgr->allocate_object(LTYPE_BOOLEAN);
                bool val = true;
                b->set_val(&val);
                this->_cur_frame->push_opstack(b);
            }

            else {
                // LBoolean* b = new LBoolean(false);
                LBoolean* b = (LBoolean*)this->mem_mgr->allocate_object(LTYPE_BOOLEAN);
                bool val = false;
                b->set_val(&val);
                this->_cur_frame->push_opstack(b);
            }

            break;
        }

        case UNARY_NOT: {
            LObject* op = this->_cur_frame->pop_opstack();

            if(this->_is_truthy(op)){
                // LBoolean* b = new LBoolean(true);
                LBoolean* b = (LBoolean*)this->mem_mgr->allocate_object(LTYPE_BOOLEAN);
                bool val = false;
                b->set_val(&val);
                this->_cur_frame->push_opstack(b);
            }
            else {
                // LBoolean* b = new LBoolean(false);
                LBoolean* b = (LBoolean*)this->mem_mgr->allocate_object(LTYPE_BOOLEAN);
                bool val = true;
                b->set_val(&val);
                this->_cur_frame->push_opstack(b);
            }

            break;
        }

        case UNARY_NEGATIVE: {
            LObject* op = this->_cur_frame->pop_opstack();

            if(op->get_type() != LTYPE_NUMBER){
                // TODO: handle type error
            }

            // LNumber* n = new LNumber(-(op->get_val().VAL_INT));
            LNumber* n = (LNumber*)this->mem_mgr->allocate_object(LTYPE_NUMBER);
            int val = (-(op->get_val().VAL_INT));
            n->set_val(&val);

            this->_cur_frame->push_opstack(n);

            // delete op;

            break;
        }

        case STORE_LOCAL: {
            this->_cur_frame->set_local_var_at_index(
                this->_advance(),
                this->_cur_frame->pop_opstack()
            );
            break;
        }

        case STORE_GLOBAL: {
            this->_main_frame->set_local_var_at_index(
                this->_advance(),
                this->_cur_frame->pop_opstack()
            );
            break;
        }

        case BIPUSH: {
            // LNumber* n = new LNumber(this->_advance());
            LNumber* n = (LNumber*)this->mem_mgr->allocate_object(LTYPE_NUMBER);
            int val = this->_advance();
            n->set_val(&val);
            this->_cur_frame->push_opstack(n);
            break;
        }

        case LOAD_LOCAL: {
            this->_cur_frame->push_opstack(
                this->_cur_frame->get_local_var_at_index(this->_advance())
            );
            break;
        }

        case LOAD_GLOBAL: {
            this->_cur_frame->push_opstack(
                this->_main_frame->get_local_var_at_index(this->_advance())
            );
            break;
        }

        case CMPEQ: {
            LObject* r = this->_cur_frame->pop_opstack();
            LObject* l = this->_cur_frame->pop_opstack();

            LObjectType ltype = l->get_type();
            LObjectType rtype = r->get_type();

            switch(ltype){
                case LTYPE_NIL:{
                    if(rtype == LTYPE_NIL){
                        // LBoolean* b = new LBoolean(true);
                        LBoolean* b = (LBoolean*)this->mem_mgr->allocate_object(LTYPE_BOOLEAN);
                        bool val = true;
                        b->set_val(&val);

                        this->_cur_frame->push_opstack(b);
                    }

                    else {
                        // LBoolean* b = new LBoolean(false);
                        LBoolean* b = (LBoolean*)this->mem_mgr->allocate_object(LTYPE_BOOLEAN);
                        bool val = false;
                        b->set_val(&val);
                        this->_cur_frame->push_opstack(b);
                    }
                    break;
                }

                case LTYPE_BOOLEAN:{
                    if(rtype != LTYPE_BOOLEAN){
                        // LBoolean* b = new LBoolean(false);
                        LBoolean* b = (LBoolean*)this->mem_mgr->allocate_object(LTYPE_BOOLEAN);
                        bool val = false;
                        b->set_val(&val);
                        this->_cur_frame->push_opstack(b);
                    }

                    else{
                        // LBoolean* b = new LBoolean(r->get_val().VAL_BOOL == l->get_val().VAL_BOOL);
                        LBoolean* b = (LBoolean*)this->mem_mgr->allocate_object(LTYPE_BOOLEAN);
                        bool val = r->get_val().VAL_BOOL == l->get_val().VAL_BOOL;
                        b->set_val(&val);
                        this->_cur_frame->push_opstack(b);
                    }

                    break;
                }

                case LTYPE_NUMBER: {
                    if(rtype != LTYPE_NUMBER){
                        // LBoolean* b = new LBoolean(false);
                        LBoolean* b = (LBoolean*)this->mem_mgr->allocate_object(LTYPE_BOOLEAN);
                        bool val = false;
                        b->set_val(&val);

                        this->_cur_frame->push_opstack(b);
                    }

                    else{
                        // LBoolean* b = new LBoolean(r->get_val().VAL_INT == l->get_val().VAL_INT);
                        LBoolean* b = (LBoolean*)this->mem_mgr->allocate_object(LTYPE_BOOLEAN);
                        bool val = r->get_val().VAL_INT == l->get_val().VAL_INT;
                        b->set_val(&val);

                        this->_cur_frame->push_opstack(b);
                    }
                    break;
                }

                case LTYPE_STRING: {
                    if(rtype != LTYPE_STRING){
                        // LBoolean* b = new LBoolean(false);
                        LBoolean* b = (LBoolean*)this->mem_mgr->allocate_object(LTYPE_BOOLEAN);
                        bool val = false;
                        b->set_val(&val);

                        this->_cur_frame->push_opstack(b);
                    }

                    else{
                        // LBoolean* b = new LBoolean(kernel::strcmp(r->get_val().VAL_STR, l->get_val().VAL_STR) == 0);
                        LBoolean* b = (LBoolean*)this->mem_mgr->allocate_object(LTYPE_BOOLEAN);
                        bool val = kernel::strcmp(r->get_val().VAL_STR, l->get_val().VAL_STR) == 0;
                        b->set_val(&val);

                        this->_cur_frame->push_opstack(b);
                    }

                    break;
                }

                case LTYPE_ARRAY: {
                    // TODO: handle type err
                    break;
                }

                // case LTYPE_FUNCTION: {
                //     // TODO: handle type err
                //     break;
                // }
            }

            // delete r;
            // delete l;

            break;
        }

        case CMPNE: {
            LObject* r = this->_cur_frame->pop_opstack();
            LObject* l = this->_cur_frame->pop_opstack();

            LObjectType ltype = l->get_type();
            LObjectType rtype = r->get_type();

            switch(ltype){
                case LTYPE_NIL:{
                    if(rtype != LTYPE_NIL){
                        // LBoolean* b = new LBoolean(true);
                        LBoolean* b = (LBoolean*)this->mem_mgr->allocate_object(LTYPE_BOOLEAN);
                        bool val = true;
                        b->set_val(&val);

                        this->_cur_frame->push_opstack(b);
                    }

                    else {
                        // LBoolean* b = new LBoolean(false);
                        LBoolean* b = (LBoolean*)this->mem_mgr->allocate_object(LTYPE_BOOLEAN);
                        bool val = false;
                        b->set_val(&val);

                        this->_cur_frame->push_opstack(b);
                    }
                    break;
                }

                case LTYPE_BOOLEAN:{
                    if(rtype != LTYPE_BOOLEAN){
                        // LBoolean* b = new LBoolean(true);
                        LBoolean* b = (LBoolean*)this->mem_mgr->allocate_object(LTYPE_BOOLEAN);
                        bool val = true;
                        b->set_val(&val);

                        this->_cur_frame->push_opstack(b);
                    }

                    else{
                        // LBoolean* b = new LBoolean(r->get_val().VAL_BOOL != l->get_val().VAL_BOOL);
                        LBoolean* b = (LBoolean*)this->mem_mgr->allocate_object(LTYPE_BOOLEAN);
                        bool val = r->get_val().VAL_BOOL != l->get_val().VAL_BOOL;
                        b->set_val(&val);

                        this->_cur_frame->push_opstack(b);
                    }

                    break;
                }

                case LTYPE_NUMBER: {
                    if(rtype != LTYPE_NUMBER){
                        // LBoolean* b = new LBoolean(true);
                        LBoolean* b = (LBoolean*)this->mem_mgr->allocate_object(LTYPE_BOOLEAN);
                        bool val = true;
                        b->set_val(&val);

                        this->_cur_frame->push_opstack(b);
                    }

                    else{
                        // LBoolean* b = new LBoolean(r->get_val().VAL_INT != l->get_val().VAL_INT);
                        LBoolean* b = (LBoolean*)this->mem_mgr->allocate_object(LTYPE_BOOLEAN);
                        bool val = r->get_val().VAL_INT != l->get_val().VAL_INT;
                        b->set_val(&val);

                        this->_cur_frame->push_opstack(b);
                    }
                    break;
                }

                case LTYPE_STRING: {
                    if(rtype != LTYPE_STRING){
                        // LBoolean* b = new LBoolean(true);
                        LBoolean* b = (LBoolean*)this->mem_mgr->allocate_object(LTYPE_BOOLEAN);
                        bool val = true;
                        b->set_val(&val);

                        this->_cur_frame->push_opstack(b);
                    }

                    else{
                        // LBoolean* b = new LBoolean(kernel::strcmp(r->get_val().VAL_STR, l->get_val().VAL_STR) != 0);
                        LBoolean* b = (LBoolean*)this->mem_mgr->allocate_object(LTYPE_BOOLEAN);
                        bool val = kernel::strcmp(r->get_val().VAL_STR, l->get_val().VAL_STR) != 0;
                        b->set_val(&val);

                        this->_cur_frame->push_opstack(b);
                    }

                    break;
                }

                case LTYPE_ARRAY: {
                    // TODO: handle type err
                    break;
                }

                // case LTYPE_FUNCTION: {
                //     // TODO: handle type err
                //     break;
                // }
            }

            // delete r;
            // delete l;

            break;
        }

        case CMPGT: {
            LObject* r = this->_cur_frame->pop_opstack();
            LObject* l = this->_cur_frame->pop_opstack();

            LObjectType ltype = l->get_type();
            LObjectType rtype = r->get_type();

            switch(ltype){

                case LTYPE_NUMBER: {
                    if(rtype != LTYPE_NUMBER){
                        // TODO: handle type err
                    }

                    else{
                        // LBoolean* b = new LBoolean(l->get_val().VAL_INT > r->get_val().VAL_INT);
                        LBoolean* b = (LBoolean*)this->mem_mgr->allocate_object(LTYPE_BOOLEAN);
                        bool val = l->get_val().VAL_INT > r->get_val().VAL_INT;
                        b->set_val(&val);

                        this->_cur_frame->push_opstack(b);
                    }
                    break;
                }

                default: {
                    // TODO: handle type err
                    break;
                }
            }

            // delete r;
            // delete l;

            break;
        }

        case CMPLT: {
            LObject* r = this->_cur_frame->pop_opstack();
            LObject* l = this->_cur_frame->pop_opstack();

            LObjectType ltype = l->get_type();
            LObjectType rtype = r->get_type();

            switch(ltype){

                case LTYPE_NUMBER: {
                    if(rtype != LTYPE_NUMBER){
                        // TODO: handle type err
                    }

                    else{
                        // LBoolean* b = new LBoolean(l->get_val().VAL_INT < r->get_val().VAL_INT);
                        LBoolean* b = (LBoolean*)this->mem_mgr->allocate_object(LTYPE_BOOLEAN);
                        bool val = l->get_val().VAL_INT < r->get_val().VAL_INT;
                        b->set_val(&val);

                        this->_cur_frame->push_opstack(b);
                    }
                    break;
                }

                default: {
                    // TODO: handle type err
                    break;
                }
            }

            // delete r;
            // delete l;

            break;
        }

        case CMPGE: {
            LObject* r = this->_cur_frame->pop_opstack();
            LObject* l = this->_cur_frame->pop_opstack();

            LObjectType ltype = l->get_type();
            LObjectType rtype = r->get_type();

            switch(ltype){

                case LTYPE_NUMBER: {
                    if(rtype != LTYPE_NUMBER){
                        // TODO: handle type err
                    }

                    else{
                        // LBoolean* b = new LBoolean(l->get_val().VAL_INT >= r->get_val().VAL_INT);
                        LBoolean* b = (LBoolean*)this->mem_mgr->allocate_object(LTYPE_BOOLEAN);
                        bool val = l->get_val().VAL_INT >= r->get_val().VAL_INT;
                        b->set_val(&val);

                        this->_cur_frame->push_opstack(b);
                    }
                    break;
                }

                default: {
                    // TODO: handle type err
                    break;
                }
            }

            // delete r;
            // delete l;

            break;
        }

        case CMPLE: {
            LObject* r = this->_cur_frame->pop_opstack();
            LObject* l = this->_cur_frame->pop_opstack();

            LObjectType ltype = l->get_type();
            LObjectType rtype = r->get_type();

            switch(ltype){

                case LTYPE_NUMBER: {
                    if(rtype != LTYPE_NUMBER){
                        // TODO: handle type err
                    }

                    else{
                        // LBoolean* b = new LBoolean(l->get_val().VAL_INT <= r->get_val().VAL_INT);
                        LBoolean* b = (LBoolean*)this->mem_mgr->allocate_object(LTYPE_BOOLEAN);
                        bool val = l->get_val().VAL_INT <= r->get_val().VAL_INT;
                        b->set_val(&val);

                        this->_cur_frame->push_opstack(b);
                    }
                    break;
                }

                default: {
                    // TODO: handle type err
                    break;
                }
            }

            // delete r;
            // delete l;

            break;
        }

        case GOTO: {
            kernel::size_t loc = (this->_advance() << 8) + this->_advance();
            this->_goto(loc);
            break;
        }

        case POP_JMP_IF_TRUE: {
            kernel::size_t idx = (this->_advance() << 8) + this->_advance();

            if(this->_is_truthy(this->_cur_frame->pop_opstack())){
                this->_goto(idx);
            }

            else {
                this->_advance();
            }

            break;
        }

        case POP_JMP_IF_FALSE: {
            kernel::size_t idx = (this->_advance() << 8) + this->_advance();

            if(!this->_is_truthy(this->_cur_frame->pop_opstack())){
                this->_goto(idx);
            }

            else {
                this->_advance();
            }
            
            break;
        }

        case CALL_FUNCTION: {
            kernel::size_t idx = this->_advance();
            func_info* fn_info = this->_code_obj->getFromFP(idx);

            Frame* f = new Frame(this->_cur_frame);
            this->mem_mgr->add_frame_to_delete_at_end(f);

            f->set_return_adress(this->_ip);

            this->_ip = -1;
            this->_cur_frame->reset();

            this->mem_mgr->add_frame_to_delete_at_end(this->_cur_frame);
            
            this->_cur_frame->set_code(fn_info->code);

            if(kernel::strcmp(f->get_name(), "main") == 0){
                this->_main_frame->set_local_vars(f->get_local_vars());
            }

            for(kernel::size_t i = 0; i < fn_info->argc; i++){
                this->_cur_frame->push_opstack(f->pop_opstack());
            }

            this->_push_frame(f);

            break;
        }

        case CALL_NATIVE: {
            kernel::size_t idx = this->_advance();

            arglist_t* argv = new kernel::Vector<LObject*>();
            kernel::u8 argc = get_argc_at_idx(idx);

            for(kernel::size_t i = 0; i < argc; i++){
                argv->push(this->_cur_frame->pop_opstack());
            }

            LObject* retval = (get_builtin_at_idx(idx))(argv);
            this->mem_mgr->add_object(retval);

            this->_cur_frame->push_opstack(retval);

            delete argv;

            break;
        }

        case RETURN_VALUE: {
            LObject* ret_val = this->_cur_frame->pop_opstack();

            //! possible exception here??
            Frame* ret_f = this->_pop_frame();
            this->_ip = ret_f->get_return_adress();
            
            // //? More efficient way to do this?
            this->_cur_frame = new Frame(ret_f);
            this->mem_mgr->add_frame_to_delete_at_end(this->_cur_frame);

            this->_cur_frame->push_opstack(ret_val);
            break;
        }

        case BUILD_LIST: {
            kernel::size_t len = (this->_advance() << 8) + this->_advance();
            LArray* arr = new LArray();
            this->mem_mgr->add_object(arr);

            kernel::Stack<LObject*>* arrList = new kernel::Stack<LObject*>();
            for(kernel::size_t i = 0; i < len; i++){
                arrList->push(this->_cur_frame->pop_opstack());
            }
            for(kernel::size_t i = 0; i < len; i++){
                arr->addEl(arrList->pop());
            }
            delete arrList;

            this->_cur_frame->push_opstack(arr);

            break;
        }

        case BINARY_SUBSCR: {
            LObject* idx_obj = this->_cur_frame->pop_opstack();

            if(idx_obj->get_type() != LTYPE_NUMBER){
                // TODO: handle type error
            }

            LNumber* idx = (LNumber*)idx_obj;
            LArray* arr = (LArray*)this->_cur_frame->pop_opstack();

            if(arr->get_type() != LTYPE_ARRAY){
                // TODO: handle type error
            }

            if((kernel::size_t)idx->get_val().VAL_INT > arr->get_val().VAL_ARRAY->get_size()){
                // TODO: handle out of bounds error
            }

            this->_cur_frame->push_opstack(
                arr->get_val().VAL_ARRAY->get_idx(idx->get_val().VAL_INT)
            );

            break;
        }

        case STORE_SUBSCR: {
            LObject* idx_obj = this->_cur_frame->pop_opstack();

            if(idx_obj->get_type() != LTYPE_NUMBER){
                // TODO: handle type error
            }

            LNumber* idx = (LNumber*)idx_obj;
            LArray* arr = (LArray*)this->_cur_frame->pop_opstack();

            if(arr->get_type() != LTYPE_ARRAY){
                // TODO: handle type error
            }

            if((kernel::size_t)idx->get_val().VAL_INT > arr->get_val().VAL_ARRAY->get_size()){
                // TODO: handle out of bounds error
            }

            LObject* val = this->_cur_frame->pop_opstack();

            arr->setEl(
                idx->get_val().VAL_INT,
                val
            );

            this->_cur_frame->push_opstack(arr);

            break;
        }

        case END: {
            break;
        }
    }
}
