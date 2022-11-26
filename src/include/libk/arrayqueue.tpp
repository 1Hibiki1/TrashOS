#include <libk/arrayqueue.h>
#include <kernel/debug.h>

template <class T>
kernel::ArrayQueue<T>::ArrayQueue(kernel::size_t sz)
    :   Array<T>(sz)
{}

template <class T>
void kernel::ArrayQueue<T>::enqueue(T item){
    this->push(item);
}

template <class T>
T kernel::ArrayQueue<T>::dequeue(){
    REQUIRES(this->get_size() > 0);
    T val = this->get_idx(0);

    for(kernel::size_t i = 0; i < (this->get_size() - 1); i++)
        this->set_idx(this->get_idx(i+1), i);

    this->set_size(this->get_size() - 1);

    return val;
}

template <class T>
T kernel::ArrayQueue<T>::get(){
    return this->get_idx(0);
}

template <class T>
T kernel::ArrayQueue<T>::get_last(){
    return this->get_idx(this->get_size() - 1);
}
