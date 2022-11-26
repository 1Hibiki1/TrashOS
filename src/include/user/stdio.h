#ifndef _STDIO_H_
#define _STDIO_H_

#include <sys/types.h>
#include <stdarg.h>
#include <unistd.h>

#define STDIN 0
#define STDOUT 1
#define STDERR 2

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
    int _fd;
    int _size;
} FILE;

extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

int printf(const char* fmt, ...);
int sprintf(char* dest, const char* fmt, ...);
int scanf(const char * format, ...);
int getchar();
ssize_t getline(char** lineptr, size_t* n, FILE* stream);
void input(char* buf);
int puts(const char *s);
int putchar(int c);

FILE *fopen(const char* pathname, const char * mode);
int fclose(FILE *stream);
char *fgets(char* s, int n, FILE* stream);
int fgetc(FILE* stream);
int fprintf(FILE* stream, const char* format, ...);
int fputs(const char * s, FILE * stream);
int fflush(FILE *stream);
size_t fread(void * ptr, size_t size, size_t nmemb,
                    FILE * stream);

int vfprintf(FILE * stream, const char * format,
           va_list ap);

#ifdef __cplusplus
}
#endif

#endif
