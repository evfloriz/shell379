#ifndef HELPERS_H
#define HELPERS_H

char **split_string(char *string, int *num_args);
int is_number(char *s);
char *convert_command_to_string(char **args, int num_args);

#endif