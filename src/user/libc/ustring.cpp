#include <string.h>
#include <stdlib.h>
#include <libk/string.h>

// https://aticleworld.com/memset-in-c/
void* memset(void *s, int c,  unsigned int len){
    unsigned char* p = (unsigned char*)s;
    while(len--)
    {
        *p++ = (unsigned char)c;
    }
    return s;
}

// https://aticleworld.com/memmove-function-implementation-in-c/
void* memmove(void* dest, const void* src, unsigned int n)
{
    char *pDest = (char *)dest;
    const char *pSrc =( const char*)src;
    //allocate memory for tmp array
    char *tmp  = (char*)malloc(sizeof(char ) * n);
    if(nullptr == tmp)
    {
        return nullptr;
    }
    else
    {
        unsigned int i = 0;
        // copy src to tmp array
        for(i =0; i < n ; ++i)
        {
            *(tmp + i) = *(pSrc + i);
        }
        //copy tmp to dest
        for(i =0 ; i < n ; ++i)
        {
            *(pDest + i) = *(tmp + i);
        }
        free(tmp); //free allocated memory
    }
    return dest;
}

// https://aticleworld.com/memcmp-in-c/
int memcmp(const void *s1, const void *s2, int len)
{
    unsigned char *p = (unsigned char *)s1;
    unsigned char *q = (unsigned char *)s2;
    int charCompareStatus = 0;
    //If both pointer pointing same memory block
    if (s1 == s2)
    {
        return charCompareStatus;
    }
    while (len > 0)
    {
        if (*p != *q)
        {  //compare the mismatching character
            charCompareStatus = (*p >*q)?1:-1;
            break;
        }
        len--;
        p++;
        q++;
    }
    return charCompareStatus;
}

void *memcpy(void * dest, const void * src, size_t n){
    char* csrc = (char*)src;
    char* cdest = (char*)dest;

    for (size_t i=0; i<n; i++){
        cdest[i] = csrc[i];
    }

    return dest;
}

size_t  strlen(const char* s){
    return kernel::strlen(s);
}

// https://aticleworld.com/strcat-in-c/
char *strcat(char * s1, const char * s2){
    //Pointer should not null pointer
    if((s1 == NULL) && (s2 == NULL))
        return (char*)NULL;
    //Create copy of s1
    char *start = s1;
    //Find the end of the destination string
    while(*start != '\0')
    {
        start++;
    }
    //Now append the source string characters
    //until not get null character of s2
    while(*s2 != '\0')
    {
        *start++ = *s2++;
    }
    //Append null character in the last
    *start = '\0';
    return s1;
}

// https://www.geeksforgeeks.org/write-your-own-atoi/
int atoi(const char *nptr){
    int res = 0;

    for (int i = 0; nptr[i] != '\0'; ++i)
        res = res * 10 + nptr[i] - '0';
 
    // return result.
    return res;
}

int strcmp(const char *s1, const char *s2){
    return kernel::strcmp(s1, s2);
}

int strncmp(const char *s1, const char *s2, size_t n){
    return kernel::strncmp(s1, s2, n);
}


char *strcpy(char * dest, const char *src){
    kernel::strcpy(dest, src);
    return dest;
}

char *strncpy(char * dest, const char * src, size_t n){
    return kernel::strncpy(dest, src, n);
}
