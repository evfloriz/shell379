#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

char **split_string(char *string, int *num_args) {
    // https://stackoverflow.com/questions/9210528/split-string-with-delimiters-in-c
    
    char **result;
    char *count_string = strdup(string);
    
    // run strtok to count how many tokens
    int count = 0;
    char *count_token;

    count_token = strtok(count_string, " ");
    while (count_token != NULL) {
        count_token = strtok(NULL, " ");
        count++;
    }
    free(count_string);

    *num_args = count;

    // allocate the right number of strings, adding one for NULL
    result = malloc(sizeof(char*) * (count + 1));

    // iterate through strings and use strtok to set each string to a token
    if (result) {
        int index = 0;
        char *token;

        token = strtok(string, " ");
        while (token != NULL) {
            result[index] = token;
            token = strtok(NULL, " ");
            index++;
        }

        // add NULL to the last value
        result[index] = NULL;
    }

    return result;
}

int is_number(char *s) {
    int i=0;
    while (s[i] != '\0') {
        if (isdigit(s[i]) == 0) {
            return 0;
        }
        i++;
    }
    return 1;
}

char *convert_command_to_string(char **args, int num_args) {
    // return null if no args (shouldn't happen)
    if (num_args == 0) {
        return "\0";
    }
    
    int size = 0;
    for (int i=0; i<num_args; i++) {
        // add length of arg + 1 for space
        size += strlen(args[i]) + 1;
    }
    
    // add +1 for null
    char *command_string = malloc(size + 1 * sizeof(char));
    
    // check for malloc failure
    if (command_string == NULL) {
        return "\0";
    }

    command_string[0] = '\0';

    for (int i=0; i<num_args; i++) {
        strcat(command_string, args[i]);
        strcat(command_string, " ");
    }
    strcat(command_string, "\0");

    return command_string;
}

// Process control block
struct Process {
    int number;
    int pid;
    char status;
    int time;
    char *command;
};

struct Process **processes;

void free_memory(char **command) {
    free(command);
    
    int i = 0;
    while (processes[i] != NULL) {
        free(processes[i]->command);
        free(processes[i]);
        i++;
    }

    free(processes);
}

// signal handler
void sigchld_handler(int sig, siginfo_t *info, void *context) {
    // https://stackoverflow.com/questions/2595503/determine-pid-of-terminated-process

    pid_t pid = info->si_pid;
    int status;

    waitpid(pid, &status, WNOHANG);
}

// handle each kind of command

int call_exit(char **command) {
    // exit program

    //
    // kill every process i think
    free_memory(command);

    exit(0);
}
int call_jobs() {
    // print jobs from the table
    printf(" # |     PID | S | SEC | COMMAND\n");
    
    int i = 0;
    while (processes[i] != NULL) {
        printf("%2d | %7d | %c | %3d | %s\n",
            i,
            processes[i]->pid,
            processes[i]->status,
            processes[i]->time,
            processes[i]->command);

        i++;
    }
        

    return 0;
}
int call_kill(int arg) {
    printf("kill %d called\n", arg);

    int i = 0;
    while (processes[i] != NULL) {
        if (processes[i]->pid == arg) {
            // kill process
            
            // this should be moved to a terminate interrupt handler
            // remove from pcb
            // move each element greater down 1
            int pos = i;

            free(processes[pos]->command);
            free(processes[pos]);
            

            while (processes[pos] != NULL) {
                processes[pos] = processes[pos + 1];
                pos++;
            }
            return 0;
        }
        i++;
    }

    return -1;

    // send kill signal to the right pid


    return 0;
}
int call_resume(int arg) {
    printf("resume %d called\n", arg);
    // send suspend signal to the right pid


    return 0;
}
int call_sleep(int arg) {
    printf("sleep %d called\n", arg);
    // call sleep for the specified number of seconds


    return 0;
}
int call_suspend(int arg) {
    printf("suspend %d called\n", arg);

    // send suspend signal to the right pid

    return 0;
}
int call_wait(int arg) {
    printf("wait %d called\n", arg);

    // if you aren't the right pid, wait until that pid is completed

    return 0;
}
int call_command(char **args, int num_args) {
    

    // need to fork a process and add it to the process control block
    int child_pid = fork();
    if (child_pid < 0) {
        perror("fork problem: ");
        _exit(1);
    }
    else if (child_pid == 0) {
        //printf("executing child with pid: %d\n", (int) getpid());
        //if (execve(args[0], args, path_env) < 0) {
        if (execvp(args[0], args) < 0) {
            perror("exec problem: ");
            _exit(1);
        }
    }

    // also need to parse any args for piping and running in the background

    // parent resumes execution

    // add to process control block
    struct Process *process = malloc(sizeof(struct Process));
    process->pid = child_pid;
    process->status = 'R';
    process->time = 0;
    process->command = convert_command_to_string(args, num_args);

    // code to find the last process and add it to the list
    // refactor a bit
    int process_index=0;
    while (processes[process_index] != NULL) {
        process_index++;
    }
    
    // need a process number?
    //process->number = process_index;
    // try changing up pid
    //process->pid = process_index + 12;
    
    processes[process_index] = process;
    processes[process_index+1] = NULL;

    /*printf("command called with %d args\n", num_args);
    for (int i=0; i<num_args; i++) {
            printf(" %d: %s\n", i, args[i]);
    }*/

    //free(full_path)

    return 0;
}

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
    //sa.sa_handler = sigchld_handler;
    sa.sa_sigaction = sigchld_handler;
    sigaction(SIGCHLD, &sa, NULL);
    
    /*if (signal(SIGCHLD, sigchld_handler) == SIG_ERR) {
        perror("Can't catch SIGCHLD");
    }*/


    printf("Welcome to shell379.\n");
    printf("PID: %d.\n", (int)getpid());

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