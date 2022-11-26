#pragma once

#include <user/posix/sys/types.h>
#include <user/posix/fcntl.h>
#include <user/posix/limits.h>
#include <user/posix/unistd.h>
#include <user/posix/sys/wait.h>
#include <user/posix/signal.h>

#ifdef __cplusplus
extern "C" {
#endif

int _open(const char *path, int oflag);
ssize_t _read(int fildes, void *buf, size_t nbyte);
off_t _lseek(int fildes, off_t offset, int whence);
ssize_t _write(int fildes, const void *buf, size_t nbyte);
int _close(int fildes);
void _exit(int status);

pid_t _wait(int *wstatus);
pid_t _waitpid(pid_t pid, int *wstatus, int options);
int _waitid(idtype_t idtype, id_t id, siginfo_t *infop, int options);

pid_t _fork(void);
pid_t _exec(const char *pathname);

int _chdir(const char *path);
char* _getcwd(char *buf, size_t size);
int _access(const char *pathname, int mode);

int _readdirent(const char* dirname, char* buf);
int _dirsize(const char* dirname);

pid_t _create_process(void (*entry)());
int _set_fg_pid(pid_t pid);

void _reload_dirs();


#ifdef __cplusplus
}
#endif
