#pragma once

#include <libk/vector.h>

// TODO: classes with one pointer makes no sense, too much memory wasted

enum LObjectType{
    LTYPE_NUMBER,
    LTYPE_STRING,
    LTYPE_NIL,
    LTYPE_BOOLEAN,
    LTYPE_ARRAY,
};

class LObject;
union LObjectValue{
    int VAL_INT;
    char* VAL_STR;
    bool VAL_BOOL;
    kernel::Vector<LObject*>* VAL_ARRAY;
};

class LObject{
    public:
        LObjectType get_type(){
            return this->type;
        }

        LObjectValue get_val(){
            return this->value;
        }

        void set_val(void* val){
            switch(this->type){
                case LTYPE_NIL:{
                    return;
                }
                case LTYPE_BOOLEAN:{
                    this->value.VAL_BOOL = *(bool*)val;
                    return;
                }
                case LTYPE_NUMBER:{
                    this->value.VAL_INT = *(int*)val;
                    return;
                }
                case LTYPE_STRING:{
                    this->value.VAL_STR = (char*)val;
                    return;
                }
                case LTYPE_ARRAY:{
                    this->value.VAL_ARRAY = (kernel::Vector<LObject*>*)val;
                    return;
                }
            }
        }
    protected:
        LObjectType type;
        LObjectValue value;
};

class LNumber : public LObject{
    public:
        LNumber(int val = 0){
            this->type = LTYPE_NUMBER;
            this->value.VAL_INT = val;
        }

        ~LNumber(){}
};

class LNil : public LObject{
    public:
        // void copy(LObject* obj) override {}
};

class LBoolean : public LObject{
    public:
        LBoolean(bool val = true){
            this->type = LTYPE_BOOLEAN;
            this->value.VAL_BOOL = val;
        }

        ~LBoolean(){}
};

class LString : public LObject{
    public:
        LString(const char* str = nullptr){
            this->type = LTYPE_STRING;
            this->value.VAL_STR = (char*)str;
        }

        ~LString(){}
};

class LArray : public LObject{
    public:
        LArray(){
            this->type = LTYPE_ARRAY;
            this->value.VAL_ARRAY = new kernel::Vector<LObject*>();


        }

        ~LArray(){
            delete this->value.VAL_ARRAY;
        }

        void addEl(LObject* el){
            this->value.VAL_ARRAY->push(el);
        }

        void setEl(kernel::size_t idx, LObject* el){
            this->value.VAL_ARRAY->set_idx(idx, el);
        }

        kernel::size_t getLen(){
            return this->value.VAL_ARRAY->get_size();
        }
};
