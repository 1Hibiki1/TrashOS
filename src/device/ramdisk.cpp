#include <device/ramdisk.h>
#include <libk/mem.h>

RamDisk::RamDisk(
    kernel::size_t size
)
:   Disk(512)
{
    this->base_adr = (kernel::u8*)kernel::malloc(size);
}


void RamDisk::read_blk(kernel::u8* buf, kernel::u8* adr){
;    for(kernel::size_t i = 0; i < this->min_blk_sz; i++){
        buf[i] = *(adr + i);
    }
    this->set_rw_head((kernel::size_t)(adr + this->min_blk_sz));
}

void RamDisk::write_blk(kernel::u8* buf, kernel::u8* adr){
    for(kernel::size_t i = 0; i < this->min_blk_sz; i++){
        adr[i] = *(buf + i);
    }
    this->set_rw_head((kernel::size_t)(adr + this->min_blk_sz));
}
