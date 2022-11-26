#ifndef _DEBUG_H_
#define _DEBUG_H_

#define DEBUG

#include <kernel/panic.h>
#include <libk/stdio.h>

#ifdef DEBUG
#define ASSERT(x) if(!(x)) kernel::panic()
#define dbg_print(...) kernel::printf(__VA_ARGS__)
#else
#define ASSERT(x) do{}while(0)
#define dbg_print(...)
#endif

#define REQUIRES(x) ASSERT(x)
#define ENSURES(x) ASSERT(x)


namespace kernel{
    void log_text(const char* fmt, ...);
    void log_info(const char* fmt, ...);
    void log_warning(const char* fmt, ...);
    void log_error(const char* fmt, ...);
};

#endif