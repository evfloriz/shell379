#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

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

/*char *print_command(char **args, num_args) {
    int size = 0;
    for (int i=0; i<num_args; i++) {
        // add length of arg + 1 for space or null
        size += strlen(args[i]) + 1;
    }
    
    char *command = malloc((strlen(path_value) + strlen(path_init)) * sizeof(char));
    
    // check for malloc failure
    if (full_path == NULL) {
        return -1;
    }

    strcpy(full_path, path_init);
    strcat(full_path, path_value);
}*/

// Process control block
/*struct Process {
    int number;
    int pid;
    char status;
    int time;
    char **command;
};*/

// handle each kind of command

int call_exit() {
    // exit program

    //
    // kill every process i think

    exit(0);
}
int call_jobs() {
    printf("jobs called\n");

    // print jobs from the table



    return 0;
}
int call_kill(int arg) {
    printf("kill %d called\n", arg);

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
    // needs to execute any unix command or executable in $PATH

    // https://stackoverflow.com/questions/8465006/how-do-i-concatenate-two-strings-in-c

    char *path_value = getenv("PATH");
    char *path_init = "PATH=";

    char *full_path = malloc((strlen(path_value) + strlen(path_init)) * sizeof(char));
    
    // check for malloc failure
    if (full_path == NULL) {
        return -1;
    }

    strcpy(full_path, path_init);
    strcat(full_path, path_value);

    //printf("%s\n", full_path);

    //char *path_env[] = {path, NULL};

    // need to fork a process and add it to the process control block

    // also need to parse any args for piping and running in the background

    // add to process control block
    /*struct Process *process = malloc(sizeof(struct Process));
    process->number = 0;
    process->pid = 12;
    process->status = 'Z';
    process->time = 0;
    process->command = args;*/

    printf("command called with %d args\n", num_args);
    for (int i=0; i<num_args; i++) {
            printf(" %d: %s\n", i, args[i]);
    }
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
            // bit of a hacky free, not sure how to do this better.
            free(command);
            success = call_exit();
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

    return success;    
}

int main() {
    int n = 255;
    char input[n];

    printf("Welcome to shell379. \n");

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

        free(command);
    }

    return 0;

}