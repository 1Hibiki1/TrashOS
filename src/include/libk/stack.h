#pragma once

#include <libk/vector.h>

namespace kernel{
    template <class T>
    class Stack{
        public:
            Stack(){
                this->_vec = new kernel::Vector<T>();
            }

            Stack(kernel::Stack<T>* s){
                this->_vec = new kernel::Vector<T>(s->_vec);
            }

            ~Stack(){
                delete this->_vec;
            }

            void push(T el){
                this->_vec->push(el);
            }

            T pop(){
                return this->_vec->pop();
            }

            T peek(){
                return this->_vec->get_idx(this->_vec->get_size() - 1);
            }

            kernel::size_t get_size(){
                return this->_vec->get_size();
            }

            kernel::Vector<T>* get_internal_vector(){
                return this->_vec;
            }
        private:
            kernel::Vector<T>* _vec;
    };
};
