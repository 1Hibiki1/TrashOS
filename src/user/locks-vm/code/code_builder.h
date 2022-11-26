#pragma once

#include "code.h"

using code_array_t = kernel::Vector<kernel::u8>;

class CodeBuilder{
    public:
        CodeBuilder(
            code_array_t* codeArr
        )
        :   _code_array(codeArr)
        {
            this->_code = new Code();
        }

        Code* get_code_obj();
        
    private:
        void _remove_from_front(kernel::size_t n);
        void _init_code();
        void _make_const_pool();
        void _make_integer();
        void _make_string();
        void _make_func_pool();
        func_info* _make_func();

        Code* _code;
        code_array_t* _code_array;
};
