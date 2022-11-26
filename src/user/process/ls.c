#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <os/file.h>
#include <unistd.h>
#include <stdbool.h>

int ls_main(){
    printf("\n");
    char cwd[20];
    char name[20];

    getcwd(cwd, 20);
    int cwd_s = strlen(cwd);

    char prev[20];

    while(readdirent(cwd, name) > -1){
        bool is_dir = false;
        char* start = &name[0];

        int i = 0;
        while(i < cwd_s && cwd[i] == name[i]){
            i++;
            start++;
        }

        if(start[0] == '/')
            start++;
        
        int start_sz = strlen(start);

        i = 0;
        while(i < start_sz){
            if(start[i] == '/'){
                start[i] = '\0';
                is_dir = true;
                break;
            }
            i++;
        }

        if(strcmp(prev, start) == 0){
            continue;
        }

        strcpy(prev, start);
        if(is_dir)
            printf("DIR     ");
        else
            printf("FILE    ");

        printf(start);
        printf("\n");
    }

    printf("\n");

    exit(0);
}