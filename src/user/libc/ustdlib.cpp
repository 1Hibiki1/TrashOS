#include <user/stdlib.h>
#include <libk/mem.h>
#include <kernel/syscall.h>
#include "../kernel/umm_malloc/umm_malloc.h"
#include <string.h>

void *malloc(size_t size){
    return kernel::malloc(size);
}

void free(void *ptr){
    kernel::free(ptr);
}

void *calloc(size_t nmemb, size_t size){
    return umm_calloc(nmemb, size);
}

void *realloc(void *ptr, size_t size){
    return umm_realloc(ptr, size);
}

void *memcpy(void* dest, void * src, size_t n){
    kernel::memcpy(dest, src, n);
    return dest;
}

void exit(int status){
    _exit(status);
}

long strtol(const char * nptr, char ** endptr, int base){
    return (long)atoi(nptr);
}
