#include <libk/array.h>
#include <libk/mem.h>
#include <kernel/debug.h>

template <class T>
kernel::Array<T>::Array(kernel::size_t sz){
    REQUIRES(sz > 0);
    this->data = (T*)kernel::malloc(sz*sizeof(T));
    this->index = 0;
    this->capacity = sz;
}

template <class T>
kernel::size_t kernel::Array<T>::get_size(){
    return this->index;
}

template <class T>
void kernel::Array<T>::set_size(kernel::size_t sz){
    this->index = sz;
}


template <class T>
kernel::size_t kernel::Array<T>::get_capcacity(){
    return this->capacity;
}

template <class T>
void kernel::Array<T>::set_idx(T el, size_t idx){
    REQUIRES(0 <= idx && idx < this->capacity);
    this->data[idx] = el;
    ENSURES(this->data[idx] == el);
}

template <class T>
T kernel::Array<T>::get_idx(size_t idx){
    REQUIRES(0 <= idx && idx < this->capacity);
    return this->data[idx];
}

template <class T>
void kernel::Array<T>::push(T el){
    REQUIRES(this->index < this->capacity);
    this->data[this->index] = el;
    this->index++;
    ENSURES(this->data[this->index-1] == el && 
            this->index <= this->capacity);
}

template <class T>
T kernel::Array<T>::pop(){
    REQUIRES(this->index > 0);
    T el = this->get_idx(this->index - 1);
    this->index--;
    return el;
}

template <class T>
void kernel::Array<T>::clear(){
    this->index = 0;
}
