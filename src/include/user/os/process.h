#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

pid_t create_process(void (*entry)());
int set_fg_pid(pid_t pid);
pid_t exec(const char *pathname);

#ifdef __cplusplus
}
#endif

#endif
