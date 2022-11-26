#include <libk/string.h>

// TODO: write contracts

void kernel::strrev(char* str){
    kernel::i32 i = 0, j = kernel::strlen(str) - 1;
    char temp;

    for(; i < j;  i++, j--){
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
    }
}

kernel::u32 kernel::strlen(const char* str){
    kernel::u32 i = 0;
    for(; str[i] != '\0'; i++);
    return i;
}


void kernel::int_to_str(kernel::i32 n, char* res, kernel::u8 is_signed){
    if(n == 0){
        res[0] = '0';
        res[1] = '\0';
        return;
    }
    
    kernel::u32 i = 0;
    kernel::u8 neg = 0;
    kernel::u32 un = 1;

    if(!is_signed){
        un = (kernel::u32)n;
    }
    else if(n < 0){
        neg = 1;
        n = -n;
    }

    if(is_signed)
        for(; n > 0; n /= 10, i++){
            res[i] = '0'+n%10;
        }
    else
        for(; un > 0; un /= 10, i++){
            res[i] = '0'+un%10;
        }

    if(neg){
        res[i++] = '-';
    }

    res[i] = '\0';

    kernel::strrev(res);
}

kernel::i32 kernel::strcmp(const char *X, const char *Y){
    while (*X){
        if (*X != *Y){
            break;
        }
        X++;
        Y++;
    }

    return *(const kernel::u8*)X - *(const kernel::u8*)Y;
}

// https://stackoverflow.com/a/32567419
kernel::i32 kernel::strncmp( const char * s1, const char * s2, kernel::size_t n ){
    while ( n && *s1 && ( *s1 == *s2 ) ){
        ++s1;
        ++s2;
        --n;
    }
    if ( n == 0 ){
        return 0;
    }
    else{
        return ( *(unsigned char *)s1 - *(unsigned char *)s2 );
    }
}


void kernel::strcpy(char* destination, const char* source){
    while (*source != '\0'){
        *destination = *source;
        destination++;
        source++;
    }
 
    *destination = '\0';
}

// from https://www.geeksforgeeks.org/program-decimal-hexadecimal-conversion/
void kernel::hex_to_str(kernel::u32 n, char* res){
    // ans string to store hexadecimal number
    if(n == 0){
        res[0] = '0';
        res[1] = '\0';
        return;
    }

    kernel::size_t i = 0;
    while (n != 0) {
        // remainder variable to store remainder
        int rem = 0;
         
        // ch variable to store each character
        char ch;
        // storing remainder in rem variable.
        rem = n % 16;
 
        // check if temp < 10
        if (rem < 10) {
            ch = rem + 48;
        }
        else {
            ch = rem + 55;
        }
         
        // updating the ans string with the character variable
        res[i++] = ch;
        n = n / 16;
    }
    res[i] = '\0';
     
    // reversing the ans string to get the final result
    kernel::strrev(res);
}

// https://www.techiedelight.com/implement-strncpy-function-c/
char* kernel::strncpy(char* destination, const char* source, kernel::size_t num)
{
    // return if no memory is allocated to the destination
    if (destination == nullptr) {
        return nullptr;
    }
 
    // take a pointer pointing to the beginning of the destination string
    char* ptr = destination;
 
    // copy first `num` characters of C-string pointed by source
    // into the array pointed by destination
    while (*source && num--)
    {
        *destination = *source;
        destination++;
        source++;
    }
 
    // null terminate destination string
    *destination = '\0';
 
    // the destination is returned by standard `strncpy()`
    return ptr;
}
