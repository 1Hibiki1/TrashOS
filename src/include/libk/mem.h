#ifndef _KMEM_H_
#define _KMEM_H_

#include <libk/stdint.h>

namespace kernel {
    void mem_init();

    void* malloc(kernel::u32 size);
    void free(void* ptr);
    void* realloc(void* ptr, kernel::size_t sz);
    kernel::u32 get_available_mem();

    void memcpy(void *dest, void *src, kernel::u32 n);
};

#endif
