#include <unistd.h>
#include <kernel/syscall.h>

int  open(const char * path, int m, ...){
    return _open(path, m);
}

ssize_t read(int fd, void *buf, size_t count){
    return _read(fd, buf, count);
}

off_t lseek(int fd, off_t offset, int whence){
    return _lseek(fd, offset, whence);
}

ssize_t write(int fd, const void *buf, size_t count){
    return _write(fd, buf, count);
}

int close(int fd){
    return _close(fd);
}

extern "C" int readdirent(const char* dirname, char* buf){
    return _readdirent(dirname, buf);
}

int chdir(const char *path){
    return _chdir(path);
}

char* getcwd(char *buf, size_t size){
    return _getcwd(buf, size);
}

int access(const char *pathname, int mode){
    return _access(pathname, mode);
}
