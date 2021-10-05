#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "helpers.h"

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

