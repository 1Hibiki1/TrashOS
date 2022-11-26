#ifndef _FCNTL_H_
#define _FCNTL_H_

// https://pubs.opengroup.org/onlinepubs/000095399/basedefs/fcntl.h.html

#ifdef __cplusplus
extern "C" {
#endif

#define O_RDONLY    1<<0
#define O_WRONLY    1<<1
#define O_RDWR      1<<2  
#define O_NONBLOCK  1<<3
#define O_APPEND    1<<4
#define O_CREAT     1<<5
#define O_TRUNC     1<<6
#define O_EXCL      1<<7

int  open(const char * path, int oflag, ...);

#ifdef __cplusplus
}
#endif

#endif
