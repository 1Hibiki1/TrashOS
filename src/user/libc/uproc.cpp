#include <os/process.h>
#include <kernel/syscall.h>
#include <unistd.h>

pid_t create_process(void (*entry)()){
    return _create_process(entry);
}

int set_fg_pid(pid_t pid){
    return _set_fg_pid(pid);
}

pid_t exec(const char *pathname){
    return _exec(pathname);
}

pid_t fork(void){
    return _fork();
}

pid_t waitpid(pid_t pid, int *wstatus, int options){
    return _waitpid(pid, wstatus, options);
}
