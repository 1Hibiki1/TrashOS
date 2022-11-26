#include <os/process.h>
#include <os/file.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "../programs/tictactoe.h"

/* Misc manifest constants */
#define MAXLINE_TSH    100   /* max line size */
#define MAXARGS     5   /* max args on a command line */
#define MAXJOBS      3   /* max jobs at any point in time */
#define MAXJID    1<<16   /* max job ID */

/* Job states */
#define UNDEF         0   /* undefined */
#define FG            1   /* running in foreground */
#define BG            2   /* running in background */
#define ST            3   /* stopped */

struct job_t {              /* The job struct */
    pid_t pid;              /* job PID */
    int jid;                /* job ID [1, 2, ...] */
    int state;              /* UNDEF, BG, FG, or ST */
    char cmdline[MAXLINE_TSH];  /* command line */
};

struct job_t* job_list; /* The job list */

void process_input(char* inp);

void shell_main(){
    size_t buf_sz = 200;
    char* buf = (char*)malloc(buf_sz);

    char name[20];

    while(1){
        getcwd(name, 20);
        printf(name);
        printf("> ");
        getline(&buf, &buf_sz, stdin);
        if(buf[0] == '\n'){
            continue;
        }
        process_input(buf);
    }
}

void run_proc(char* path, bool bg){
    pid_t p = exec(path);
    if(!bg){
        set_fg_pid(p);
        waitpid(p, NULL, 0);
    }
    else {
        printf("background job running...\n\n");
    }
}

void process_input(char* inp){
    // remove trailing newline
    size_t inp_len = strlen(inp);
    inp[inp_len-1] = '\0';
    inp_len--;

    bool is_bg = false;

    if(inp[inp_len - 1] == '&'){
        is_bg = true;
        if(inp[inp_len - 2] == ' '){
            inp[inp_len - 2] = '\0';
        }
        else{
            inp[inp_len - 1] = '\0';
        }
    }

    if(strncmp(inp, "cd", 2) == 0){
        if(strlen(inp) < 4){
            inp += 2;
            printf(inp);
            printf(" : no such directory\n\n");
            return;
        }

        inp += 3;

        if(chdir(inp) < 0){
            char cur_path[20];
            getcwd(cur_path, 20);
            strcat(cur_path, inp);

            if(chdir(cur_path) < 0){
                printf(inp);
                printf(" : no such directory\n\n");
            }
            return;
        }

    }

    else if(strcmp(inp, "clear") == 0){
        // https://stackoverflow.com/a/17271636
        printf("\033[H\033[J");
    }

    else if(strncmp(inp, "cat ", 4) == 0){
        inp += 4;
        int fd = open(inp, 0);

        if(fd < 0){
            printf("unable to open ");
            printf(inp);
            printf("\n");
            return;
        }

        char* buf = malloc(512);
        read(fd, buf, 512);

        printf(buf);

        close(fd);

        free(buf);
        return;
    }

    else {
        if(access(inp, F_OK) == 0){
            run_proc(inp, is_bg);
            return;
        }

        char cur_path[20];
        getcwd(cur_path, 20);
        strcat(cur_path, inp);

        if(access(cur_path, F_OK) == 0){
            run_proc(cur_path, is_bg);
            return;
        }


        char full_path[20];
        strcpy(full_path, "/bin/");
        strcat(full_path, inp);

        if(access(full_path, F_OK) == 0){
            run_proc(full_path, is_bg);
            return;
        }

        printf(inp);
        printf(" : no such command or executable\n\n");
    }
}

/* fgpid - Return PID of current foreground job, 0 if no such job */
pid_t fgpid(struct job_t *job_list) {
    int i;

    for (i = 0; i < MAXJOBS; i++)
        if (job_list[i].state == FG)
            return job_list[i].pid;
    return 0;
}
