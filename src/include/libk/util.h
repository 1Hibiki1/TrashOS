#ifndef _UTIL_H_
#define _UTIL_H_

#include <libk/stdint.h>

namespace kernel {
    kernel::size_t read_from_adress(kernel::size_t* adr){
        return (*adr);
    }

    void write_to_adress(kernel::size_t data, volatile kernel::size_t* adr){
        *adr = data;
    }

    kernel::size_t* add_offset_to_adr(kernel::size_t* adr, kernel::u32 offset){
        return (kernel::size_t*)((u8*)adr + offset);
    }
};

#endif
