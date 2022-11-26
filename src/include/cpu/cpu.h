#ifndef _CPU_H_
#define _CPU_H_

#include <cpu/port.h>
#include <cpu/tm4c.h>
#include <libk/stdint.h>

struct __attribute__((packed)) CPU_state{
    kernel::u32 R4;
    kernel::u32 R5;
    kernel::u32 R6;
    kernel::u32 R7;
    kernel::u32 R8;
    kernel::u32 R9;
    kernel::u32 R10;
    kernel::u32 R11;
    kernel::u32 R14;

    kernel::u32 R0;
    kernel::u32 R1;
    kernel::u32 R2;
    kernel::u32 R3;
    kernel::u32 R12;
    kernel::u32 LR;
    kernel::u32 PC;
    kernel::u32 xPSR;
};

class CPU{
    public:
        Port* portA;
        Port* portB;
        Port* portC;
        Port* portD;
        Port* portE;
        Port* portF;

        CPU();
        void enable_interrupts(){
            __asm volatile ("cpsie if  \n");
        }
        void disable_interrupts(){
            __asm volatile ("cpsid if  \n");
        }
    private:
};

#endif