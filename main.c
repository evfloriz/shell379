#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/resource.h>

#include "helpers.h"
#include "process.h"
#include "handler.h"

int parse(char **command, int num_args) {
    // interate through tokens
    // if first token is exit or jobs, make sure there's no other args and then launch
    // if first token is kill, resume, sleep, suspend, wait, make sure there's a second int arg and nothing else, then launch
    // if first token is anything else, send it the command parser

    if (num_args == 0) {
        return 0;
    }
    
    char *first_token = command[0];
    int is_invalid = 0;
    int success = 0;

    if (strcmp(first_token, "exit") == 0) {
        if (num_args != 1) {
            is_invalid = 1;
        }
        else {
            success = call_exit(command);
        }
    }
    else if (strcmp(first_token, "jobs") == 0) {
        if (num_args != 1) {
            is_invalid = 1;
        }
        else {
            success = call_jobs();
        }
    }
    else if (strcmp(first_token, "kill") == 0) {
        if (num_args != 2 || !is_number(command[1])) {
            is_invalid = 1;
        }
        else {
            success = call_kill(atoi(command[1]));
        }
    }
    else if (strcmp(first_token, "resume") == 0) {
        if (num_args != 2 || !is_number(command[1])) {
            is_invalid = 1;
        }
        else {
            success = call_resume(atoi(command[1]));
        }
        
    }
    else if (strcmp(first_token, "sleep") == 0) {
        if (num_args != 2 || !is_number(command[1])) {
            is_invalid = 1;
        }
        else {
            success = call_sleep(atoi(command[1]));
        }
        
    }
    else if (strcmp(first_token, "suspend") == 0) {
        if (num_args != 2 || !is_number(command[1])) {
            is_invalid = 1;
        }
        else {
            success = call_suspend(atoi(command[1]));
        }
        
    }
    else if (strcmp(first_token, "wait") == 0) {
        if (num_args != 2 || !is_number(command[1])) {
            is_invalid = 1;
        }
        else {
            success = call_wait(atoi(command[1]));
        }
    }
    else {
        success = call_command(command, num_args);
    }

    if (is_invalid) {
        printf("Invalid command\n");
        success = 1;
    }

    free(command);

    return success;    
}

int main() {
    int n = 255;
    char input[n];

    // allocate max number of processes + 1 for null
    processes = malloc(sizeof(struct Process*) * 33);
    processes[0] = NULL;

    // initialize sig_chld handler
    struct sigaction sa;
    // init sa
    //sa.sa_flags = 0;
    // specify restart in case interrupt happens while waiting for fgets
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = sigchld_handler;
    sigaction(SIGCHLD, &sa, NULL);
    
    /*if (signal(SIGCHLD, sigchld_handler) == SIG_ERR) {
        perror("Can't catch SIGCHLD");
    }*/


    printf("Welcome to shell379.\n");

    // replace this with 1 once things are working, just making sure I don't fork bomb accidentally
    int limiter = 0;
    while (limiter++ < 10) {
        // get input
        fgets(input, n, stdin);

        // change newline char to null char
        int input_length = strlen(input);
        if (input_length > 0 && input[input_length-1] == '\n') {
            input[input_length-1] = '\0';
        }

        char **command;
        int num_args = 0;

        // split string
        command = split_string(input, &num_args);

        parse(command, num_args);

        //free(command);
    }

    // for now, recursively free every process
    // should be done in exit only once there's a infinite loop
    int count_process = 0;
    while (processes[count_process] != NULL) {
        free(processes[count_process]->command);
        free(processes[count_process]);
        count_process++;
    }

    free(processes);

    return 0;

}