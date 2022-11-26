#ifndef _SERIAL_H_
#define _SERIAL_H_

#include <device/device.h>

enum serial_color{
    SERIAL_FG_RED, SERIAL_FG_GREEN,
    SERIAL_FG_BLUE, SERIAL_FG_YELLOW,
    SERIAL_FG_WHITE
};

// TODO: make this work independent of w and h
/* 
should we print " " for moving the cursor right and '\b for moving the
cursor left? so we wouldnt have to keep track of the cursor position
-> but then how would we move the cursor "up"?
*/
class Serial : public GenericDevice {
    public:
        Serial(kernel::size_t w, kernel::size_t h);

        kernel::size_t get_width();
        kernel::size_t get_height();

        void flush(kernel::size_t s);
        void flush();
        void puts(const char* str);

        void set_color(serial_color col);

        void set_cursor(kernel::size_t x, kernel::size_t y);
        void set_cursor_x(kernel::size_t x);
        void set_cursor_y(kernel::size_t y);
        
        void move_cursor_right();
        void move_cursor_left();
        void move_cursor_up();
        void move_cursor_down();

        void clrscr();

    private:
        kernel::size_t width;
        kernel::size_t height;
        kernel::size_t cursor_x;
        kernel::size_t cursor_y;
};

#endif
