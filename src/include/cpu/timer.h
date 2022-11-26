#ifndef _TIMER_H_
#define _TIMER_H_

#include <libk/stdint.h>

#define NUM_TIMERS 2
enum timer_type{
    TIMER_SYSTICK
};

// This class seems to have a size of...1 byte?
// it would be a waste to malloc lol
class Timer {
    public:
        Timer(
            timer_type t,
            kernel::size_t reload_value
        );

        void set_reload_value(kernel::size_t reload_val);
        void set_callback(void (*handler)(void));
        void start();
        void stop();
    private:
        timer_type type;
};

#endif
