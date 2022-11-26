#include <user/stdio.h>
#include <user/posix/sys/types.h>

#include <libk/string.h>
#include <string.h>
#include <stdlib.h>
#include <libk/stdio.h>
#include <libk/mem.h>

#include <kernel/syscall.h>

#include <cstdarg>

FILE* stdin;
FILE* stdout;
FILE* stderr;


//! FIXME: return the number of chars printed
int printf(const char* fmt, ...){
    va_list args;
    va_start(args, fmt);

    int fmt_len = kernel::strlen(fmt);
    int fmt_num = 0;
    for(int i = 0; i < fmt_len; i++){
        if(fmt[i] == '%')
            fmt_num++;
    }

    char* str = (char*)kernel::malloc(fmt_len + fmt_num*12 + 1);
    kernel::_sprintf_va_args(str, fmt, args);
    kernel::log_text(str);
    kernel::free(str);

    va_end(args);

    return 0;
}

int puts(const char *s){
    return printf(s);
}

int putchar(int c){
    char temp[] = {(char)c, '\0'};
    return printf(temp);
}

int sprintf(char* dest, const char* fmt, ...){
    va_list args;
    va_start(args, fmt);

    kernel::_sprintf_va_args(dest, fmt, args);

    va_end(args);

    return 0;
}

// https://iq.opengenus.org/how-printf-and-scanf-function-works-in-c-internally/
int scanf (const char * str, ...)
{
    va_list vl;
    int i = 0, j=0, ret = 0;
    char buff[100] = {0}, c = '\0';
    char *out_loc;
    while(c != '\n') 
    {
        c = getchar();
        buff[i] = c;
        i++;
 	}
    buff[i-1] = '\0';
 	va_start( vl, str );
 	i = 0;
 	while (str && str[i])
 	{
 	    if (str[i] == '%') 
 	    {
 	       i++;
 	       switch (str[i]) 
 	       {
 	           case 'c': 
 	           {
	 	           *(char *)va_arg( vl, char* ) = buff[j];
	 	           j++;
	 	           ret ++;
	 	           break;
 	           }
 	           case 'd': 
 	           {
	 	           *(int *)va_arg( vl, int* ) = strtol(&buff[j], &out_loc, 10);
	 	           j+=out_loc -&buff[j];
	 	           ret++;
	 	           break;
 	            }
 	            case 'x': 
 	            {
	 	           *(int *)va_arg( vl, int* ) = strtol(&buff[j], &out_loc, 16);
	 	           j+=out_loc -&buff[j];
	 	           ret++;
	 	           break;
 	            }
 	        }
 	    } 
 	    else 
 	    {
 	        buff[j] =str[i];
            j++;
        }
        i++;
    }
    va_end(vl);
    return ret;
}

size_t fread(void * ptr, size_t size, size_t nmemb, FILE * stream){
    return 0;
}

int getchar(){
    lseek(STDIN, 0, SEEK_SET);

    char null_buf[] = {0, 0, 0, 0};
    write(STDIN, null_buf, 1);

    while(null_buf[0] == 0){
        lseek(STDIN, 0, SEEK_SET);
        read(STDIN, null_buf, 1);
    }

    if(null_buf[0] != '\b'){
        write(STDOUT, null_buf, 2);
    }

    return null_buf[0];
}

ssize_t getline(char** lineptr, size_t* n, FILE* stream){
    size_t i = 0;
    size_t pos = 0;
    if(stream == stdin){
        char c = '\0';
        for(;i < *n && c != '\n'; i++){
            c = getchar();
            if(c == '\b'){
                if(pos == 0){
                    continue;
                }
                else{
                    pos--;
                }
                char space_buf[] = {'\b', ' ', '\b', 0};
                write(STDOUT, space_buf, 3);
                i -= 2;
            }
            else{
                (*lineptr)[i] = c;
                pos++;
            }
        }
    }

    (*lineptr)[i] = '\0';
    return i;
}

void input(char* buf){
    char inp = '\0';
    int i = 0;

    while(inp != '\n'){
        inp = getchar();
        buf[i++] = inp;
    }
    buf[i] = '\0';
}


FILE *fopen(const char * pathname, const char * mode){
    return nullptr;
}

int fclose(FILE *stream){
    return -1;
}

char *fgets(char * s, int n, FILE * stream){
    return nullptr;
}

int fgetc(FILE *stream){
    return -1;
}

int fputs(const char * s, FILE * stream){
    if(stream == stdout || stream == stderr){
        kernel::log_text(s);
        return strlen(s);
    }
    return -1;
}

int fflush(FILE *stream){
    return 0;
}

int fprintf(FILE* stream, const char * format, ...){
    if(stream == stdout || stream == stderr){
        va_list args;
        va_start(args, format);

        int fmt_len = kernel::strlen(format);
        int fmt_num = 0;
        for(int i = 0; i < fmt_len; i++){
            if(format[i] == '%')
                fmt_num++;
        }

        char* str = (char*)kernel::malloc(fmt_len + fmt_num*12);
        kernel::_sprintf_va_args(str, format, args);
        kernel::log_text(str);
        kernel::free(str);

        va_end(args);

        return 0;
    }
    return -1;
}

int vfprintf(FILE * stream, const char * format, va_list ap){
    if(stream == stdout || stream == stderr){

        int fmt_len = kernel::strlen(format);
        int fmt_num = 0;
        for(int i = 0; i < fmt_len; i++){
            if(format[i] == '%')
                fmt_num++;
        }

        char* str = (char*)kernel::malloc(fmt_len + fmt_num*12);
        kernel::_sprintf_va_args(str, format, ap);
        kernel::log_text(str);
        kernel::free(str);

        return 0;
    }
    return -1;
}
