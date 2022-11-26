#include <kernel/debug.h>
#include <kernel/kernel.h>
#include <device/serial.h>
#include <device/request.h>
#include <libk/string.h>

// TODO: change all strings declared on stack to malloc'd

void kernel::log_text(const char* fmt, ...){
    IOManager* iomgr = kernel::get_iomgr();

    kernel::get_scheduler()->pause();
    Serial* ser = kernel::get_serial_device();
    ser->set_color(SERIAL_FG_WHITE);
    kernel::get_scheduler()->run();

    char val[200];
    va_list x;
    va_start(x, fmt);
    kernel::_sprintf_va_args(val, fmt, x);

    Request req(
        REQ_WRITE,
        (GenericDevice*)kernel::get_serial_device(),
        kernel::get_scheduler()->get_cur_proc(),
        (kernel::u8*)val,
        kernel::strlen(val)
    );

    iomgr->add_request(&req);

    while(req.get_process()->get_state() == P_STATE_WAITING)
        __asm("nop  \n");
    va_end(x);
}

void kernel::log_info(const char* fmt, ...){
    Serial* ser = kernel::get_serial_device();

    ser->set_color(SERIAL_FG_GREEN);

    char* val = (char*)kernel::malloc(kernel::strlen(fmt)+100);

    va_list x;
    va_start(x, fmt);
    kernel::_sprintf_va_args(val, fmt, x);

    ser->puts(val);
    kernel::free(val);
    va_end(x);
}

void kernel::log_warning(const char* fmt, ...){
    Serial* ser = kernel::get_serial_device();

    ser->set_color(SERIAL_FG_YELLOW);

    char* val = (char*)kernel::malloc(kernel::strlen(fmt)+100);

    va_list x;
    va_start(x, fmt);
    kernel::_sprintf_va_args(val, fmt, x);

    ser->puts(val);
    kernel::free(val);
    va_end(x);
}

void kernel::log_error(const char* fmt, ...){
    Serial* ser = kernel::get_serial_device();

    ser->set_color(SERIAL_FG_RED);

    char* val = (char*)kernel::malloc(kernel::strlen(fmt)+100);

    va_list x;
    va_start(x, fmt);
    kernel::_sprintf_va_args(val, fmt, x);

    ser->puts(val);
    kernel::free(val);
    va_end(x);
}
