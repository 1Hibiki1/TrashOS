#pragma once

#include <libk/vector.h>

namespace kernel{
    template <class T>
    class Set{
        public:
            Set(){
                this->_vec = new kernel::Vector<T>();
            }

            ~Set(){
                delete this->_vec;
            }

            kernel::size_t get_size(){
                return this->_vec->get_size();
            }

            T get(kernel::size_t i){
                return this->_vec->get_idx(i);
            }

            bool exists(T el){
                for(kernel::size_t i = 0; i < this->_vec->get_size(); i++){
                    if(this->_vec->get_idx(i) == el){
                        return true;
                    }
                }
                return false;
            }

            void add(T el){
                if(!this->exists(el)){
                    this->_vec->push(el);
                }
            }
        private:
            kernel::Vector<T>* _vec;
    };
};
