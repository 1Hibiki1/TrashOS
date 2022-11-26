#include <cpu/cpu.h>
#include <libk/mem.h>
#include <cpu/tm4c.h>

CPU::CPU(){
    // init PLL
    SYSCTL_RCC2_R |= 0x80000000;
    SYSCTL_RCC2_R |= 0x00000800;
    SYSCTL_RCC_R = (SYSCTL_RCC_R & ~0x000007C0) + 0x00000540;
    SYSCTL_RCC2_R &= ~0x00000070;
    SYSCTL_RCC2_R &= ~0x00002000;
    SYSCTL_RCC2_R |= 0x40000000;
    SYSCTL_RCC2_R = (SYSCTL_RCC2_R & ~0x1FC00000) + (4 << 22);
    while ((SYSCTL_RIS_R &0x00000040)==0){};
    SYSCTL_RCC2_R &= ~0x00000800;

    // TODO: do we really need to allocate mem for all ports?
    portA = new Port((kernel::size_t*)GPIO_PORTA_DATA_BITS_R);
    portB = new Port((kernel::size_t*)GPIO_PORTB_DATA_BITS_R);
    portC = new Port((kernel::size_t*)GPIO_PORTC_DATA_BITS_R);
    portD = new Port((kernel::size_t*)GPIO_PORTD_DATA_BITS_R);
    portE = new Port((kernel::size_t*)GPIO_PORTE_DATA_BITS_R);
    portF = new Port((kernel::size_t*)GPIO_PORTF_DATA_BITS_R);

    // set pendsv priority to highest???    -> sus
    NVIC_SYS_PRI3_R &= ~(0b111 << 21);
}
