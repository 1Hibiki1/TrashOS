#ifndef _DISK_H_
#define _DISK_H_

#include <libk/stdint.h>
#include <device/device.h>

class Disk : public GenericDevice{
    public:
        Disk(
            kernel::size_t mbs
        )
        :   GenericDevice(mbs, mbs),
            min_blk_sz(mbs),
            base_adr(nullptr),
            rw_head(0)
        {}

        virtual ~Disk(){}

        void set_rw_head(kernel::size_t val){
            this->rw_head = val;
        }

        kernel::size_t get_rw_head(){
            return this->rw_head;
        }

        void set_base_adr(kernel::u8* badr){
            this->base_adr = badr;
        }
        
        virtual void read_blk(kernel::u8* buf, kernel::u8* adr) = 0;
        virtual void write_blk(kernel::u8* buf, kernel::u8* adr) = 0;

        void read_nblk(kernel::u8* buf, kernel::size_t blk_no){
            this->read_blk(buf, this->base_adr+(blk_no*this->min_blk_sz));
        }

        void write_nblk(kernel::u8* buf, kernel::size_t blk_no){
            this->write_blk(buf, this->base_adr+(blk_no*this->min_blk_sz));
        }

        kernel::size_t get_blk_sz(){
            return this->min_blk_sz;
        }
    protected:
        kernel::size_t min_blk_sz;
        kernel::u8* base_adr;
        kernel::size_t rw_head;
};

#endif
