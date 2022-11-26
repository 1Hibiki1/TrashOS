#include <libk/stdio.h>
#include <libk/stdint.h>
#include <libk/string.h>
#include <kernel/kernel.h>
#include <cstdarg>

void kernel::_sprintf_va_args(char* dest, const char* fmt, va_list args){
    char cur_char;

    
    for(kernel::size_t i = 0; fmt[i] != '\0'; i++, dest++){
        cur_char = fmt[i];

        if(cur_char == '%'){
            char mod = fmt[i+1];
            if(mod == 'd'){
                char temp_i[12];
                kernel::int_to_str(va_arg(args, kernel::i32), temp_i, 1);
                kernel::strcpy(dest, temp_i);
                dest += kernel::strlen(temp_i) - 1;
            }

            else if(mod == 'u'){
                char temp_i[12];
                kernel::int_to_str(va_arg(args, kernel::u32), temp_i, 0);
                kernel::strcpy(dest, temp_i);
                dest += kernel::strlen(temp_i) - 1;
            }

            else if(mod == 'x'){
                char temp_i[12];
                kernel::hex_to_str(va_arg(args, kernel::u32), temp_i);
                kernel::strcpy(dest, temp_i);
                dest += kernel::strlen(temp_i) - 1;
            }

            else if(mod == 'c'){
                *dest = (char)va_arg(args, int);
            }

            else {
                *dest = '%';
                dest++;
            }

            i++;
        }

        else
            *dest = cur_char;
    }
    
    *dest = '\0';
}

void kernel::sprintf(char* dest, const char* fmt, ...){
    va_list args;
    va_start(args, fmt);

    kernel::_sprintf_va_args(dest, fmt, args);

    va_end(args);
}
