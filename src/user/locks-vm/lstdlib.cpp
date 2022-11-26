#include "lstdlib.h"
#include <libk/string.h>
#include <user/string.h>
#include <user/stdio.h>
#include "mem/vmmem.h"

LObject* (*builtin_function_table[])(arglist_t*) = {
    &locks_print,
    &locks_println,
    &locks_input,
    &locks_len,
    &locks_int,
    &locks_str,
    &locks_is_integer,
};

kernel::u8 argc_table[] = {
    1,
    1,
    1,
    1,
    1,
    1,
    1
};

builtin_fun_t get_builtin_at_idx(kernel::size_t idx){
    return builtin_function_table[idx];
}

kernel::u8 get_argc_at_idx(kernel::size_t idx){
    return argc_table[idx];
}

LObject* locks_print(arglist_t* arglist){
    LObject* arg1 = arglist->get_idx(0);

    LObjectType type = arg1->get_type();

    switch(type){
        case LTYPE_STRING: {
            const char* str = arg1->get_val().VAL_STR;
            printf(str);
            break;
        }

        case LTYPE_NUMBER: {
            int num = arg1->get_val().VAL_INT;
            char str[12];
            kernel::int_to_str(num, str, true);
            printf(str);
            break;
        };

        case LTYPE_NIL: {
            printf("nil");
            break;
        }

        default: {
            return nullptr;
        }
    }


    LNil* n = new LNil();
    return n;
}

LObject* locks_println(arglist_t* arglist){
    LObject* arg1 = arglist->get_idx(0);

    LObjectType type = arg1->get_type();

    switch(type){
        case LTYPE_STRING: {
            const char* str = arg1->get_val().VAL_STR;
            printf(str);
            printf("\n");
            break;
        }

        case LTYPE_NUMBER: {
            int num = arg1->get_val().VAL_INT;
            char str[12];
            kernel::int_to_str(num, str, true);
            printf(str);
            printf("\n");
            break;
        };

        case LTYPE_NIL: {
            printf("nil\n");
            break;
        }

        default: {
            return nullptr;
        }
    }


    LNil* n = new LNil();
    return n;
}

LObject* locks_input(arglist_t* arglist){
    LObject* arg1 = arglist->get_idx(0);

    LObjectType type = arg1->get_type();
    char* res = (char*)kernel::malloc(100);
    size_t sz = 100;

    switch(type){
        case LTYPE_STRING: {
            const char* str = arg1->get_val().VAL_STR;
            printf(str);
            getline(&res, &sz, stdin);
            break;
        }

        case LTYPE_NUMBER: {
            int x = arg1->get_val().VAL_INT;
            printf("%d", x);
            // getline(&res, &sz, stdin);
            input(res);
            break;
        }

        default: {
            return nullptr;
        }
    }

    LString* ret_val = new LString(res);
    return ret_val;
}

LObject* locks_len(arglist_t* arglist){
    LObject* arg1 = arglist->get_idx(0);
    LObjectType type = arg1->get_type();

    // TODO: handle string len

    if(type != LTYPE_ARRAY){
        //TODO: handle type error
    }

    LArray* arr = (LArray*)arg1;
    LNumber* n = new LNumber(arr->getLen());

    return n;
}

LObject* locks_int(arglist_t* arglist){
    LObject* arg1 = arglist->get_idx(0);
    LObjectType type = arg1->get_type();

    switch(type){
        case LTYPE_NIL: {
            LNumber* n = new LNumber(0);
            return n;
            break;
        }

        case LTYPE_BOOLEAN: {
            LNumber* n = new LNumber(int(arg1->get_val().VAL_BOOL));
            return n;
            break;
        }

        case LTYPE_NUMBER: {
            return arg1;
            break;
        }

        case LTYPE_STRING: {
            char* str_val = ((LString*)arg1)->get_val().VAL_STR;
            int num_val = atoi(str_val);
            LNumber* n = new LNumber(num_val);
            return n;
            break;
        }

        default:{
        //TODO: handle type error
        }
    }

    return nullptr;
}

LObject* locks_str(arglist_t* arglist){
    LObject* arg1 = arglist->get_idx(0);
    LObjectType type = arg1->get_type();

    switch(type){
        case LTYPE_NIL: {
            LString* n = new LString("nil");
            return n;
            break;
        }

        case LTYPE_BOOLEAN: {
            LString* n = nullptr;

            if((int(arg1->get_val().VAL_BOOL)) == true){
                n = new LString("true");
            }
            else {
                n = new LString("false");
            }
            return n;
            break;
        }

        case LTYPE_NUMBER: {
            int n = arg1->get_val().VAL_INT;
            char* str = (char*)kernel::malloc(12);
            kernel::int_to_str(n, str, 1);

            LString* str_obj = new LString((const char*)str);
            return str_obj;
            break;
        }

        case LTYPE_STRING: {
            return arg1;
            break;
        }

        default:{
        //TODO: handle type error
        }
    }

    return nullptr;
}

LObject* locks_is_integer(arglist_t* arglist){
    LObject* arg1 = arglist->get_idx(0);
    LObjectType type = arg1->get_type();

    bool is_true = false;
    if(type == LTYPE_NUMBER){
        is_true = true;
    }

    LBoolean* res = new LBoolean(is_true);
 
    return res;
}
