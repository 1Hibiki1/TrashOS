#include <libk/stdint.h>

using namespace kernel;

extern "C" void kmain(void);

//.text/code,stored in Flash
extern int _etext;
//.data,copied into RAM on boot
extern int _data;
extern int _edata;
//.bss,unitialized variables
extern int _bss;
extern int _ebss;

extern "C" void Reset_Handler(void) {
    // stack pointer is already set at end of sram

    /* copying of the .data values into RAM */
    i32 *src = &_etext;
    for (i32* dest = &_data; dest < &_edata;) {
        *dest++ = *src++;
    }

    /* initializing .bss values to zero*/
    for (i32* dest = &_bss; dest < &_ebss;) {
        *dest++ = 0;
    }

    /* your program's main() called */
    kmain();

    // ? Do I need this? It's just habit at this point.
    while (1);
}
