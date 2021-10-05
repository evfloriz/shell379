#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>

#include "process.h"
#include "helpers.h"

// remove process from pcb
int remove_process(int pid) {
    int i = 0;
    while (processes[i] != NULL) {
        if (processes[i]->pid == pid) {
            // remove from pcb
            int pos = i;

            // free the process struct at pos
            free(processes[pos]->command);
            free(processes[pos]);

            // move each element greater than pos down 1
            while (processes[pos] != NULL) {
                processes[pos] = processes[pos + 1];
                pos++;
            }
            return 0;
        }
        i++;
    }
    return -1;
}

int update_time() {
    FILE *pfile;

    if ((pfile = popen("ps -o pid,times --no-headers", "r")) == NULL) {
        perror("Pipe open failure: ");
        return -1;
    }
    
    int n = 128;
    char ps_line[n];
    char **times;
    int num_args = 2;
    while (fgets(ps_line, n, pfile) != NULL) {
        times = split_string(ps_line, &num_args);
        
        int i = 0;
        while (processes[i] != NULL) {
            // if pid matches, update seconds
            if (processes[i]->pid == atoi(times[0])) {
                processes[i]->time = atoi(times[1]);
            }
            i++;
        }
    }
    if (pclose(pfile) < 0) {
        perror("Pipe close failure: ");
    }    

    return 0;
}

int update_process_status(int pid, char status) {
    int i = 0;
    while (processes[i] != NULL) {
        if (processes[i]->pid == pid) {
            processes[i]->status = status;
            return 0;
        }
    }
    return -1;
}