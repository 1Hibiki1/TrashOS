#ifndef _ARRAY_H_
#define _ARRAY_H_

#include <libk/mem.h>
#include <kernel/debug.h>

namespace kernel {
    template <class T>
    class Array {
        public:
            Array(kernel::size_t sz);
            virtual ~Array(){
                kernel::free(this->data);
            }

            kernel::size_t get_size();
            kernel::size_t get_capcacity();
            void set_size(kernel::size_t sz);

            void set_idx(T el, kernel::size_t idx);
            void set_capacity(kernel::size_t cap){
                REQUIRES(cap > 0);
                this->capacity = cap;
            }
            T get_idx(size_t idx);

            void push(T el);
            T pop();

            void clear();

            T* get_data_ptr(){
                return data;
            }
            void set_data_ptr(T* ptr){
                this->data = ptr;
            }
        private:
            T* data;
            kernel::size_t index;
            kernel::size_t capacity;
    };
};

#include "array.tpp"

#endif
