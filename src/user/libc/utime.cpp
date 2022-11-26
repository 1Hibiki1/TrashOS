#include <time.h>
#include <kernel/kernel.h>

clock_t clock(void){
    return kernel::get_clock_ctr();
}
