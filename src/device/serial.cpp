#include <device/serial.h>
#include <cpu/tm4c.h>
#include <libk/string.h>

// TODO: add abstraction for UART

Serial::Serial(kernel::size_t w, kernel::size_t h)
    :   GenericDevice(0, 512),
        width(w),
        height(h),
        cursor_x(0),
        cursor_y(0)
{
    SYSCTL_RCGCUART_R |= 0x01;
    SYSCTL_RCGCGPIO_R |= 0x01;

    // we need a small pause here before configuring the port,
    // or it crashes with -O3
    // configuring UART first also does the job

    UART0_CTL_R &= ~UART_CTL_UARTEN;
    UART0_IBRD_R = 0x8;
    UART0_FBRD_R = 0x2C;
    UART0_LCRH_R = 0x0070;
    UART0_CTL_R |= UART_CTL_UARTEN;
    UART0_IFLS_R = 0x12;
    UART0_CC_R = 0x05;

    GPIO_PORTA_AFSEL_R |= 0x03;
    GPIO_PORTA_DEN_R |= 0x03;
    GPIO_PORTA_PCTL_R = 0x11;
}

void Serial::flush(){
    this->flush(this->out_buffer->get_capcacity());
}

void Serial::flush(kernel::size_t s){
    REQUIRES(s <= this->out_buffer->get_capcacity());

    for(kernel::size_t i = 0; i < s; i++){
        char ch = this->out_buffer->get_idx(i);
        while(UART0_FR_R & 0x20);
        UART0_DR_R = ch;
        __asm("    NOP");
        __asm("    NOP");


        if(ch == '\n'){
            while(UART0_FR_R & 0x20);
            UART0_DR_R = '\r';
            __asm("    NOP\n");
            __asm("    NOP\n");

            this->cursor_y++;
            this->cursor_x = 0;
        }
        else this->cursor_x++;
    }

    if(this->cursor_x >= this->width){
        this->cursor_y++;
        this->cursor_x -= this->width;
    }

    if(this->cursor_y >= height)
        this->cursor_y = height-1;

    this->out_buffer->clear();
}

void Serial::puts(const char* str){
    // TODO: what if string is larger than buffer size?

    kernel::size_t sz = kernel::strlen(str);
    kernel::size_t cap = this->out_buffer->get_capcacity();

    if(sz < cap){
        while(*str){
            this->write((kernel::u8)*str);
            str++;
        }
        this->flush(sz);
    }

    else {
        kernel::size_t ctr = 0;
        while(ctr < sz){
            kernel::size_t i = 0;
            for(; i < cap; i++, ctr++){
                this->write((kernel::u8)str[ctr]);
                if(str[ctr] == '\0')
                    break;
            }
            this->flush(i < cap ? i : cap);
        }
    }
}

void Serial::set_color(serial_color col){
    switch(col){
        case SERIAL_FG_RED: {
            this->puts("\033[1;31m");
            break;
        }

        case SERIAL_FG_BLUE: {
            this->puts("\033[1;34m");
            break;
        }

        case SERIAL_FG_GREEN: {
            this->puts("\033[1;32m");
            break;
        }

        case SERIAL_FG_YELLOW: {
            this->puts("\033[1;33m");
            break;
        }

        case SERIAL_FG_WHITE: {
            this->puts("\033[1;37m");
            break;
        }
    }
}


void Serial::set_cursor(kernel::size_t x, kernel::size_t y){
    char temp[] = {27, '[', '0', '1', ';', '0', '1', 'H', '\0'};
    char temp_int[3];

    this->cursor_x = x;
    this->cursor_y = y;

    // ANSI excape codes use index 1 for cursor, so get y + 1
    kernel::int_to_str(y+1, temp_int, 0);

    if(y < 10){
        temp[3] = temp_int[0];
    }else{
        temp[2] = temp_int[0];
        temp[3] = temp_int[1];
    }

    kernel::int_to_str(x+1, temp_int, 0);
    if(x < 10){
        temp[6] = temp_int[0];
    }else{
        temp[5] = temp_int[0];
        temp[6] = temp_int[1];
    }

    this->puts(temp);
}


void Serial::set_cursor_x(kernel::size_t x){
    this->set_cursor(x, this->cursor_y);
}

void Serial::set_cursor_y(kernel::size_t y){
    this->set_cursor(this->cursor_x, y);
}

void Serial::move_cursor_right(){
    if(this->cursor_x >= this->width){
        this->cursor_x = 0;
        this->move_cursor_down();
    }

    else{
        this->puts("\033[C");
        this->cursor_x++;
    }
}

void Serial::move_cursor_left(){
    if(this->cursor_x <= 0){
        this->cursor_x = width - 1;
        this->move_cursor_up();
    }

    else{
        this->puts("\033[D");
        this->cursor_x--;
    }
}

void Serial::move_cursor_up(){
    if(this->cursor_y <= 0){
        return;
    }

    else{
        this->puts("\033[A");
        this->cursor_y--;
    }
}

void Serial::move_cursor_down(){
    if(this->cursor_y >= height){
        this->puts("\n");
    }

    else{
        this->puts("\033[B");
        this->cursor_y++;
    }
}

void Serial::clrscr(){
    this->cursor_x = 0;
    this->cursor_y = 0;
    this->puts("\033[01;01H"); // set cursor to 0,0
    this->puts("\033[02J");    // clear screen
}
