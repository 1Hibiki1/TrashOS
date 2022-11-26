#pragma once

#include <libk/vector.h>
#include <libk/stack.h>
#include <libk/set.h>
#include "../types.h"
#include "../stack/frame.h"

class VMMemoryManager{
    public:
        VMMemoryManager();
        ~VMMemoryManager();

        LObject* allocate_object(LObjectType type);
    
        void nuke_garbage(kernel::Stack<Frame*>* stack);
        void nuke_useless_objects();

        void add_object(LObject* obj);
        void add_to_delete_at_end(void* item);

        void add_frame_to_delete_at_end(Frame* item);

        template <class T>
        void add_vector_to_delete_at_end(kernel::Vector<T>* item){
            this->add_array_to_delete_at_end<T>(item->get_internal_arr());
            this->delete_list->add(item);
        }
        
        template <class T>
        void add_stack_to_delete_at_end(kernel::Stack<T>* item){
            this->add_vector_to_delete_at_end<T>(item->get_internal_vector());
            this->delete_list->add(item);
        }

        template <class T>    
        void add_array_to_delete_at_end(kernel::Array<T>* item){
            this->delete_list->add(item);
            this->delete_list->add(item->get_data_ptr());
        }
    private:
        kernel::Set<LObject*>* allocated_list;
        kernel::Set<void*>* delete_list;
};
