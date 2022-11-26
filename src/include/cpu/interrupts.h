#ifndef _INTERRUPTS_H_
#define _INTERRUPTS_H_


// TODO: find a better way to do this
enum Pin {
    PA2,PA3,PA4,PA5,PA6,PA7,
    PB0,PB1,PB2,PB3,PB4,PB5,PB6,PB7,
    PC4,PC5,PC6,PC7,
    PD0,PD1,PD2,PD3,PD6,PD7,
    PE0,PE1,PE2,PE3,PE4,PE5,
    PF0,PF1,PF2,PF3,PF4,
};

enum int_mode {
    RISING,
    FALLING,
    BOTH
};

void install_handler(Pin p, void (*handler)());
void interrupts_init();

#endif