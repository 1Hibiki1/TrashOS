#ifndef _ARRAYQUEUE_H_
#define _ARRAYQUEUE_H_

#include <libk/array.h>
#include <libk/stdint.h>

namespace kernel{
    template <class T>
    class ArrayQueue : public Array<T> {
        public:
            ArrayQueue(kernel::size_t sz);
            void enqueue(T item);
            T dequeue();
            T get();
            T get_last();
        private:
    };
};

#include "arrayqueue.tpp"

#endif
