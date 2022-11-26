#pragma once

#include <libk/array.h>

#define MIN_VEC_SIZE 2

namespace kernel{
    template <class T>
    class Vector{
        public:
            Vector(){
                this->arr = new kernel::Array<T>(MIN_VEC_SIZE);
            }

            Vector(kernel::Vector<T>* v){
                this->arr = new kernel::Array<T>(MIN_VEC_SIZE);
                kernel::size_t sz = v->get_size();

                // ASSERT(sz > 0);

                for(kernel::size_t i = 0; i < sz; i++){
                    this->push(v->get_idx(i));
                }
            }

            ~Vector(){
                delete this->arr;
            }

            void push(T el){
                kernel::size_t cap = this->arr->get_capcacity();
                ASSERT(cap > 0);

                if(this->arr->get_size() >= cap){
                    kernel::size_t new_cap = (cap*2 - cap/2);
                    ASSERT(new_cap > 0);

                    T* data = arr->get_data_ptr();
                    T* new_data = (T*)kernel::realloc(data, new_cap*sizeof(T));

                    this->arr->set_data_ptr(new_data);
                    this->arr->set_capacity(new_cap);
                }
                this->arr->push(el);
            }

            T pop(){
                return arr->pop();
            }

            kernel::size_t get_size(){
                return this->arr->get_size();
            }

            kernel::size_t get_capacity(){
                return this->arr->get_capcacity();
            }


            T get_idx(kernel::size_t idx){
                return this->arr->get_idx(idx);
            }
            void set_idx(kernel::size_t idx, T el){
                this->arr->set_idx(el, idx);
            }

            T delete_idx(kernel::size_t idx){
                REQUIRES(idx < this->arr->get_size());
                T el = this->arr->get_idx(idx);
                kernel::size_t cur_sz = this->arr->get_size();

                kernel::memcpy(
                    this->arr->get_data_ptr(),  
                    this->arr->get_data_ptr() + 1,
                    cur_sz - 1  
                );

                this->arr->set_size(cur_sz - 1);
                return el;
            }
            
            kernel::Array<T>* get_internal_arr(){
                return this->arr;
            }
        private:
            kernel::Array<T>* arr;
    };
} // namespace kernel

