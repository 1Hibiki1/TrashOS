#ifndef _STRING_H_
#define _STRING_H_

#include <sys/types.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif


void *memset(void *s, int c, size_t n);
void* memmove(void* dest, const void* src, unsigned int n);
int memcmp(const void *s1, const void *s2, size_t n);
void *memcpy(void * dest, const void * src, size_t n);

int atoi(const char *nptr);

size_t strlen(const char* s);
char *strcat(char * dest, const char * src);
int strcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
char *strcpy(char * dest, const char *src);
char *strncpy(char * dest, const char * src, size_t n);

#ifdef __cplusplus
}
#endif

#endif
