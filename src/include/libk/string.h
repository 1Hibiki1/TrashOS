#ifndef _KSTRING_H_
#define _KSTRING_H_

#include <libk/stdint.h>

namespace kernel {
    kernel::u32 strlen(const char* str);
    void int_to_str(kernel::i32 n, char* res, kernel::u8 is_signed);
    void hex_to_str(kernel::u32 n, char* res);

    void strrev(char* str);
    kernel::i32 strcmp(const char* X, const char* Y);
    kernel::i32 strncmp(const char* X, const char* Y, kernel::size_t n);
    void strcpy(char* destination, const char* source);
    char* strncpy(char* destination, const char* source, size_t num);
};

#endif