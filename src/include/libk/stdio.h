#pragma once

#include <cstdarg>
#include <kernel/debug.h>

namespace kernel{
    void log_text(const char* fmt, ...);
}

namespace kernel {
    void _sprintf_va_args(char* dest, const char* fmt, va_list args);
    void sprintf(char* dest, const char* fmt, ...);
    template<typename... Args> void printf(char const* fmt, Args... args);
}; // namespace  kernel

template<typename... Args> void kernel::printf(char const* fmt, Args... args){
    kernel::log_text(fmt, args...);
}
