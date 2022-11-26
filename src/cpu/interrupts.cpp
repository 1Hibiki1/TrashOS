#include <cpu/interrupts.h>
#include <libk/stdint.h>
#include <cpu/tm4c.h>
#include <kernel/kernel.h>

void (*handler_arr[35])() = {};

void default_handler(){}

void interrupts_init(){
    for(kernel::u8 i = 0; i < 35; i++){
        handler_arr[i] = default_handler;
    }
}

void install_handler(Pin p, void (*handler)()){
    handler_arr[p] = handler;
}

// not using interrupts on these ports
void GPIOPortA_ISR(){}
void GPIOPortB_ISR(){}
void GPIOPortC_ISR(){}
void GPIOPortE_ISR(){}


extern "C" void GPIOPortD_ISR(){
    kernel::u8 im = (kernel::u8)GPIO_PORTD_MIS_R;

    for(kernel::u8 i = 0; i < 8; i++)
        if(im & (1 << i)){
            switch(i){
                case 0: {
                    handler_arr[PD0]();
                    GPIO_PORTD_ICR_R |= (1 << i);
                    break;
                }

                case 1: {
                    handler_arr[PD1]();
                    GPIO_PORTD_ICR_R |= (1 << i);
                    break;
                }

                case 2: {
                    handler_arr[PD2]();
                    GPIO_PORTD_ICR_R |= (1 << i);
                    break;
                }

                case 3: {
                    handler_arr[PD3]();
                    GPIO_PORTD_ICR_R |= (1 << i);
                    break;
                }
            }
        }
}

extern "C" void GPIOPortF_ISR(){
    kernel::u8 im = *(kernel::u8*)GPIO_PORTF_MIS_R;

    for(kernel::u8 i = 0; i < 8; i++)
        if(im & (1 << i)){
            switch(i){
                case 0: {
                    handler_arr[PF0]();
                    GPIO_PORTF_ICR_R |= (1 << i);
                    break;
                }

                case 1: {
                    handler_arr[PF1]();
                    GPIO_PORTF_ICR_R |= (1 << i);
                    break;
                }

                case 2: {
                    handler_arr[PF2]();
                    GPIO_PORTF_ICR_R |= (1 << i);
                    break;
                }

                case 3: {
                    handler_arr[PF3]();
                    GPIO_PORTF_ICR_R |= (1 << i);
                    break;
                }

                case 4: {
                    handler_arr[PF4]();
                    GPIO_PORTF_ICR_R |= (1 << i);
                    break;
                }
            }
        }
}
