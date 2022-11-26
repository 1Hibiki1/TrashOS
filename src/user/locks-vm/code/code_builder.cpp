#include "code_builder.h"

Code* CodeBuilder::get_code_obj(){
    this->_init_code();
    this->_make_const_pool();
    this->_make_func_pool();

    return this->_code;
}

void CodeBuilder::_remove_from_front(kernel::size_t n){
    // TODO: more efficient way to do this?
    for(kernel::size_t i = 0; i < n; i++){
        this->_code_array->delete_idx(0);
    }
}

void CodeBuilder::_init_code(){
    if(this->_code_array->get_size() < 10){
        // TODO: handle error
    }

    int magic = (
        (this->_code_array->get_idx(0) << 24) +
        (this->_code_array->get_idx(1) << 16) +
        (this->_code_array->get_idx(2) << 8)  +
        this->_code_array->get_idx(3)
    );

    if(magic != magic_number){
        // TODO: handle error
    }

    this->_remove_from_front(4);
}

void CodeBuilder::_make_const_pool(){
    kernel::size_t cp_count = (
        (this->_code_array->get_idx(0) << 8) + 
        this->_code_array->get_idx(1)
    );

    this->_remove_from_front(2);

    for(kernel::size_t i = 0; i < cp_count; i++){
        Tag t = (Tag)this->_code_array->get_idx(0);
        this->_remove_from_front(1);

        switch(t){
            case CONSTANT_Integer: {
                this->_make_integer();
                break;
            }

            case CONSTANT_Double:{
                //TODO: handle error
                break;
            }

            case CONSTANT_String:{
                this->_make_string();
                break;
            }

            default:{
                //TODO: handle error
            }
        }

    }
}

void CodeBuilder::_make_integer(){
    int i = (
        (this->_code_array->get_idx(4) << 24) +
        (this->_code_array->get_idx(5) << 16) +
        (this->_code_array->get_idx(6) << 8)  +
        this->_code_array->get_idx(7)
    );
    void* i_ptr = kernel::malloc(sizeof(i));
    *(int*)i_ptr = i;

    // TODO: we dont need to check for negative int ryt?

    this->_remove_from_front(8);

    cp_info* cpi = new cp_info(CONSTANT_Integer, i_ptr);
    this->_code->addToCP(cpi);
}

void CodeBuilder::_make_string(){
    kernel::Vector<char>* temp_str = new kernel::Vector<char>();
    char* str;

    kernel::size_t len = 0; 
    while(this->_code_array->get_idx(len) != 0)
        len++;

    str = (char*)kernel::malloc(len+1);

    for(kernel::size_t i = 0; i < len + 1; i++){
        str[i] = (char)this->_code_array->delete_idx(0);
    }

    cp_info* cpi = new cp_info(CONSTANT_String, str);
    this->_code->addToCP(cpi);

    delete temp_str;
}

void CodeBuilder::_make_func_pool(){
    kernel::size_t fp_count = (
        (this->_code_array->get_idx(0) << 8) +
        this->_code_array->get_idx(1)
    );

    this->_remove_from_front(2);

    for(kernel::size_t i = 0; i < fp_count; i++){
        this->_code->addToFP(this->_make_func());
    }
}

func_info* CodeBuilder::_make_func(){
    func_info* f = new func_info();

    kernel::size_t argc = (
        (this->_code_array->get_idx(0) << 8) +
        this->_code_array->get_idx(1)
    );
    this->_remove_from_front(2);
    f->argc = argc;

    kernel::size_t code_count = (
        (this->_code_array->get_idx(0) << 8) +
        this->_code_array->get_idx(1)
    );
    this->_remove_from_front(2);

    for(kernel::size_t i = 0; i < code_count; i++){
        f->code->push(this->_code_array->get_idx(0));
        this->_remove_from_front(1);
    }

    return f;
}
