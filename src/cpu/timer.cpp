#include <cpu/timer.h>
#include <cpu/tm4c.h>

void (*handler_array[NUM_TIMERS])(void);

Timer::Timer(timer_type t, kernel::size_t reload_value){
    switch(t){
        case TIMER_SYSTICK: {
            this->type = TIMER_SYSTICK;
            NVIC_ST_CTRL_R = 0;
            NVIC_ST_RELOAD_R = reload_value;
            NVIC_ST_CURRENT_R = 0;
            break;
        }
    }
}

void Timer::set_reload_value(kernel::size_t reload_value){
    switch(this->type){
        case TIMER_SYSTICK: {
            NVIC_ST_RELOAD_R = reload_value;
            break;
        }
    }
}

void Timer::set_callback(void (*handler)(void)){
    switch(this->type){
        case TIMER_SYSTICK: {
            handler_array[TIMER_SYSTICK] = handler;
            break;
        }
    }
}

void Timer::start(){
    switch(this->type){
        case TIMER_SYSTICK: {
            NVIC_ST_CTRL_R = 0x00000007;
            break;
        }
    }
}

void Timer::stop(){
    switch(this->type){
        case TIMER_SYSTICK: {
            NVIC_ST_CTRL_R = 0;
            break;
        }
    }
}

extern "C" void SysTick_Handler(){
    handler_array[TIMER_SYSTICK]();
}
