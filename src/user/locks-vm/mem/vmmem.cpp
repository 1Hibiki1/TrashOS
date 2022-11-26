#include "vmmem.h"


VMMemoryManager::VMMemoryManager(){
    this->allocated_list = new kernel::Set<LObject*>();
    this->delete_list = new kernel::Set<void*>();
}

VMMemoryManager::~VMMemoryManager(){
    delete this->allocated_list;
    delete this->delete_list;
}


LObject* VMMemoryManager::allocate_object(LObjectType type){
    switch(type){
        case LTYPE_NIL: {
            LNil* obj  = new LNil();
            this->allocated_list->add(obj);
            return obj;
        }

        case LTYPE_BOOLEAN: {
            LBoolean* obj  = new LBoolean();
            this->allocated_list->add(obj);
            return obj;
        }

        case LTYPE_NUMBER: {
            LNumber* obj  = new LNumber();
            this->allocated_list->add(obj);
            return obj;
        }

        case LTYPE_STRING: {
            LString* obj  = new LString();
            this->allocated_list->add(obj);
            return obj;
        }

        case LTYPE_ARRAY: {
            LArray* obj  = new LArray();
            this->allocated_list->add(obj);
            return obj;
        }
    }
    return nullptr;
}

void VMMemoryManager::nuke_garbage(kernel::Stack<Frame*>* stack){
    kernel::size_t sz = this->allocated_list->get_size();

    for(kernel::size_t i = 0; i < sz; i++){
        void* ptr = this->allocated_list->get(i);
        kernel::free(ptr);
    }
}

void VMMemoryManager::nuke_useless_objects(){
    for(kernel::size_t i = 0; i < this->delete_list->get_size(); i++){
        kernel::free(this->delete_list->get(i));
    }
}

void VMMemoryManager::add_object(LObject* obj){
    this->allocated_list->add(obj);
}

void VMMemoryManager::add_to_delete_at_end(void* item){
    this->delete_list->add(item);
}

void VMMemoryManager::add_frame_to_delete_at_end(Frame* item){
    this->delete_list->add(item);
    this->add_vector_to_delete_at_end<kernel::u8>(item->get_code());
    this->add_stack_to_delete_at_end<LObject*>(item->get_operand_stack());
    this->add_array_to_delete_at_end<LObject*>(item->get_local_vars());
}
