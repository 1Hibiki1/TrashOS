#ifndef _WAIT_H_
#define _WAIT_H_

// https://pubs.opengroup.org/onlinepubs/007904875/basedefs/sys/wait.h.html

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    P_ALL,
    P_PID,
    P_PGID
} idtype_t;

pid_t waitpid(pid_t pid, int *wstatus, int options);


#ifdef __cplusplus
}
#endif

#endif
