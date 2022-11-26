#include <libk/mem.h>
#include <kernel/panic.h>

void *operator new(kernel::size_t size){
    return kernel::malloc(size);
}
 
void *operator new[](kernel::size_t size){
    return kernel::malloc(size);
}
 
void operator delete(void *p){
    kernel::free(p);
}

void operator delete(void *p, kernel::size_t x){
    kernel::free(p);
}
 
void operator delete[](void *p){
    kernel::free(p);
}

// chould never be called
extern "C" void __cxa_pure_virtual(){
    kernel::panic();
}
