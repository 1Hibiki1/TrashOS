#ifndef _SIGNAL_H_
#define _SIGNAL_H_

// https://pubs.opengroup.org/onlinepubs/007904875/basedefs/signal.h.html

#include <sys/types.h>

union sigval {
    int    sival_int;
    void  *sival_ptr;
};

typedef struct {
    int           si_signo; 
    int           si_code;
    int           si_errno;
    pid_t         si_pid;
    uid_t         si_uid;
    void         *si_addr;
    int           si_status;
    long          si_band;
    union sigval  si_value;
} siginfo_t;

#endif
