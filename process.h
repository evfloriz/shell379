#ifndef PROCESS_H
#define PROCESS_H

// Process control block
struct Process {
    int number;
    int pid;
    char status;
    int time;
    char *command;
};

struct Process **processes;

int remove_process(int pid);
int update_time();
int update_process_status(int pid, char status);

#endif