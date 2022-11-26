#include <user/process/init.h>
#include <user/process/shell.h>
#include <user/programs/transferd.h>
#include <os/process.h>


void dummy_entry(){
    while(1){
        __asm("nop  \n");
    }
}

void init_main(){

    create_process(dummy_entry);
    create_process(transferd_main);
    set_fg_pid(create_process(shell_main));

    while(1){
        __asm("nop  \n");
    }
}
