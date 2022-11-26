#ifndef _TIME_H_
#define _TIME_H_

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int    tm_sec;
    int    tm_min;
    int    tm_hour;
    int    tm_mday;
    int    tm_mon;
    int    tm_year;
    int    tm_wday;
    int    tm_yday;
    int    tm_isdst;
} tm;

#define CLOCKS_PER_SEC 250

clock_t clock(void);

#ifdef __cplusplus
}
#endif

#endif
