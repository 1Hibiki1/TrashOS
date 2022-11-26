#include "frame.h"

Frame::Frame(){
    this->name = nullptr;
    this->_operand_stack = new kernel::Stack<LObject*>();
    this->_local_vars = new kernel::Array<LObject*>(MAX_NUM_LOCAL_VARS);
    this->_code = new  kernel::Vector<kernel::u8>();
}

Frame::Frame(Frame* f){
    this->name = f->name;

    this->_operand_stack = f->_operand_stack;
    this->_local_vars = f->_local_vars;

    ASSERT(f->_code->get_size() > 0);
    this->_code = f->_code;
}

Frame::Frame(const char* n)
:   name(n)
{
    this->_operand_stack = new kernel::Stack<LObject*>();
    this->_local_vars = new kernel::Array<LObject*>(MAX_NUM_LOCAL_VARS);
    this->_code =new  kernel::Vector<kernel::u8>();
}

Frame::~Frame(){
}


void Frame::push_opstack(LObject* obj){
    this->_operand_stack->push(obj);
}

LObject* Frame::pop_opstack(){
    return this->_operand_stack->pop();
}


void Frame::set_return_adress(kernel::size_t adr){
    this->_ret_address = adr;
}

kernel::size_t Frame::get_return_adress(){
    return this->_ret_address;
}


LObject* Frame::get_local_var_at_index(kernel::size_t i){
    return this->_local_vars->get_idx(i);
}

void Frame::set_local_var_at_index(kernel::size_t i, LObject* obj){
    this->_local_vars->set_idx(obj, i);
}

void Frame::set_local_vars(kernel::Array<LObject*>* vars){
    //TODO: free _local_vars?
    this->_local_vars = vars;
}

void Frame::set_code(kernel::Vector<kernel::u8>* code){
    this->_code = code;
}


kernel::u8 Frame::get_ins_at_index(kernel::size_t i){
    return this->_code->get_idx(i);
}


void Frame::reset(){
    this->name = nullptr;
    this->_operand_stack = new kernel::Stack<LObject*>();
    this->_local_vars = new kernel::Array<LObject*>(MAX_NUM_LOCAL_VARS);
}
