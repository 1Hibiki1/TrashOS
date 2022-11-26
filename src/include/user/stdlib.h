#ifndef _STDLIB_H_
#define _STDLIB_H_

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

void *malloc(size_t size);
void free(void *ptr);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void exit(int status);

long strtol(const char * nptr,
                   char ** endptr, int base);

#ifdef __cplusplus
}
#endif

#endif
