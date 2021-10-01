#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

char **splitString(char *string, int *numArgs) {
    // https://stackoverflow.com/questions/9210528/split-string-with-delimiters-in-c
    
    char **result;
    char *countString = strdup(string);
    
    // run strtok to count how many tokens
    int count = 0;
    char *countToken;

    countToken = strtok(countString, " ");
    while (countToken != NULL) {
        countToken = strtok(NULL, " ");
        count++;
    }

    *numArgs = count;

    // allocate the right number of strings
    result = malloc(sizeof(char*) * count);

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
    }

    return result;
}

int isNumber(char *s) {
    int i=0;
    while (s[i] != '\0') {
        if (isdigit(s[i]) == 0) {
            return 0;
        }
        i++;
    }
    return 1;
}

int call_exit() {
    // exit program
    exit(0);
}
int call_jobs() {
    printf("jobs called\n");
    return 0;
}
int call_kill(int arg) {
    printf("kill %d called\n", arg);
    return 0;
}
int call_resume(int arg) {
    printf("resume %d called\n", arg);
    return 0;
}
int call_sleep(int arg) {
    printf("sleep %d called\n", arg);
    return 0;
}
int call_suspend(int arg) {
    printf("suspend %d called\n", arg);
    return 0;
}
int call_wait(int arg) {
    printf("wait %d called\n", arg);
    return 0;
}
int call_command(char **args, int numArgs) {
    printf("command called with %d args\n", numArgs);
    for (int i=0; i<numArgs; i++) {
            printf(" %d: %s\n", i, args[i]);
    }
    return 0;
}

int parse(char **command, int numArgs) {
    // interate through tokens
    // if first token is exit or jobs, make sure there's no other args and then launch
    // if first token is kill, resume, sleep, suspend, wait, make sure there's a second int arg and nothing else, then launch

    if (numArgs == 0) {
        return 0;
    }
    
    char *firstToken = command[0];
    int isInvalid = 0;
    int success = 0;

    if (strcmp(firstToken, "exit") == 0) {
        if (numArgs != 1) {
            isInvalid = 1;
        }
        else {
            // bit of a hacky free, not sure how to do this better.
            free(command);
            success = call_exit();
        }
    }
    else if (strcmp(firstToken, "jobs") == 0) {
        if (numArgs != 1) {
            isInvalid = 1;
        }
        else {
            success = call_jobs();
        }
    }
    else if (strcmp(firstToken, "kill") == 0) {
        if (numArgs != 2 || !isNumber(command[1])) {
            isInvalid = 1;
        }
        else {
            success = call_kill(atoi(command[1]));
        }
    }
    else if (strcmp(firstToken, "resume") == 0) {
        if (numArgs != 2 || !isNumber(command[1])) {
            isInvalid = 1;
        }
        else {
            success = call_resume(atoi(command[1]));
        }
        
    }
    else if (strcmp(firstToken, "sleep") == 0) {
        if (numArgs != 2 || !isNumber(command[1])) {
            isInvalid = 1;
        }
        else {
            success = call_sleep(atoi(command[1]));
        }
        
    }
    else if (strcmp(firstToken, "suspend") == 0) {
        if (numArgs != 2 || !isNumber(command[1])) {
            isInvalid = 1;
        }
        else {
            success = call_suspend(atoi(command[1]));
        }
        
    }
    else if (strcmp(firstToken, "wait") == 0) {
        if (numArgs != 2 || !isNumber(command[1])) {
            isInvalid = 1;
        }
        else {
            success = call_wait(atoi(command[1]));
        }
    }
    else {
        success = call_command(command, numArgs);
    }

    if (isInvalid) {
        printf("Invalid command\n");
        success = 1;
    }

    return success;    
}

int main() {
    int n = 255;
    char input[n];

    printf("Enter a message: \n");

    while (1) {
        // get input
        fgets(input, n, stdin);

        // change newline char to null char
        int inputLength = strlen(input);
        if (inputLength > 0 && input[inputLength-1] == '\n') {
            input[inputLength-1] = '\0';
        }

        char **command;
        int numArgs = 0;

        // split string
        command = splitString(input, &numArgs);

        parse(command, numArgs);

        free(command);
    }

    return 0;

}