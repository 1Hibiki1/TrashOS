#pragma once

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SEEK_SET    0
#define SEEK_CUR    1
#define SEEK_END    2

#define F_OK 1

ssize_t read(int fd, void *buf, size_t count);
off_t lseek(int fd, off_t offset, int whence);
ssize_t write(int fd, const void *buf, size_t count);
int close(int fd);
int chdir(const char *path);
char* getcwd(char *buf, size_t size);
int access(const char *pathname, int mode);

pid_t fork(void);
int execve(const char *pathname, char *const argv[],
                  char *const envp[]);

#ifdef __cplusplus
}
#endif
