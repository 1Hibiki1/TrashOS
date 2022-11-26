#ifndef _PORT_H_
#define _PORT_H_

#include <libk/stdint.h>
#include <cpu/interrupts.h>

enum io_dir {
    INPUT,
    OUTPUT
};


class Port {
    public:

        Port(kernel::size_t* base_adr);
        void digital_init_pin(kernel::u8 pin, io_dir dir);
        void attach_interrupt_pin(Pin p, int_mode mode, void (*handler)());

        void write_byte(kernel::u8 data);
        kernel::u8 read_byte();
        void write_pin(kernel::u8 pin, kernel::u8 val);
        kernel::u8 read_pin(kernel::u8 pin);

        void set_pur(kernel::u8 p);
        void set_pdr(kernel::u8 p);

    private:

        kernel::size_t* base_adr;

        kernel::size_t* data_adr; 
        kernel::size_t* dir_adr; 
        kernel::size_t* lock_adr; 
        kernel::size_t* cr_adr; 
        kernel::size_t* amsel_adr; 
        kernel::size_t* afsel_adr; 
        kernel::size_t* pctl_adr; 
        kernel::size_t* pur_adr; 
        kernel::size_t* pdr_adr; 
        kernel::size_t* den_adr;

        kernel::size_t* is_adr;
        kernel::size_t* ibe_adr;
        kernel::size_t* iev_adr;
        kernel::size_t* icr_adr;
        kernel::size_t* im_adr;
        kernel::size_t* mis_adr;


        const kernel::u32 CLK_ADR = 0x400FE608;
        const kernel::u32 LOCK_VAL = 0x4C4F434B;

        const kernel::u32 DATA_OFFSET = 0x3FC;
        const kernel::u32 DIR_OFFSET = 0x400;
        const kernel::u32 LOCK_OFFSET = 0x520;
        const kernel::u32 CR_OFFSET = 0x524;  // gpio commit
        const kernel::u32 AMSEL_OFFSET = 0x528;  // gpio analog mode
        const kernel::u32 AFSEL_OFFSET = 0x420;  // gpio alternate functonality
        const kernel::u32 PCTL_OFFSET = 0x52C;  // gpio port control
        const kernel::u32 PUR_OFFSET = 0x510;
        const kernel::u32 PDR_OFFSET = 0x514;
        const kernel::u32 DEN_OFFSET = 0x51C;

        const kernel::u32 IS_OFFSET = 0x404;
        const kernel::u32 IBE_OFFSET = 0x408;
        const kernel::u32 IEV_OFFSET = 0x40C;
        const kernel::u32 ICR_OFFSET = 0x41C;
        const kernel::u32 IM_OFFSET = 0x410;
        const kernel::u32 MIS_OFFSET = 0x418;
};

#endif