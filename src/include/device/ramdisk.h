#ifndef _RAMDISK_H_
#define _RAMDISK_H_

#include <device/disk.h>

class RamDisk : public Disk {
    public:
        RamDisk(
            kernel::size_t size
        );

    private:
        void read_blk(kernel::u8* buf, kernel::u8* adr) override;
        void write_blk(kernel::u8* buf, kernel::u8* adr) override;
};

#endif
