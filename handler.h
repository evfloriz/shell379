#ifndef HANDLER_H
#define HANDLER_H

int call_exit(char **command);
int call_jobs();
int call_kill(int arg);
int call_suspend(int arg);
int call_resume(int arg);
int call_sleep(int arg);
int call_wait(int arg);
int call_command(char **args, int num_args);
void sigchld_handler(int sig, siginfo_t *info, void *context);

#endif