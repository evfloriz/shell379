#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sys/types.h>

#include "handler.h"
#include "process.h"
#include "helpers.h"


// handle each kind of command
int call_exit(char **command) {
    // exit program

    // should kill every process i think
    // will this mess up child edits to global process
    
    // make a list of pids
    // this is necessary because killing a process can change the global process table
    int count_pids = 0;
    while (processes[count_pids] != NULL) {
        count_pids++;
    }

    int *pids = malloc(count_pids * sizeof(int));

    for (int i=0; i<count_pids; i++) {
        pids[i] = processes[i]->pid;
    }
    
    // iterate through pids and call kill on each one
    for (int i=0; i<count_pids; i++) {
        if (call_kill(pids[i]) < 0) {
            return -1;
        }
    }

    free(pids);

    // wait until everything's deleted from the pcb
    while (processes[0] != NULL);

    free(command);
    free(processes);

    exit(0);
}
int call_jobs() {
    update_time();
    
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
    // kill process
    if (kill(arg, SIGKILL) < 0) {
        perror("Kill error: ");
        return -1;
    }

    return 0;
}
int call_suspend(int arg) {
    // suspend process
    if (kill(arg, SIGTSTP) < 0) {
        perror("Suspend error: ");
    }

    return 0;
}
int call_resume(int arg) {
    // resume process
    if (kill(arg, SIGCONT) < 0) {
        perror("Resume error: ");
    }

    return 0;
}
int call_sleep(int arg) {
    // call sleep for the specified number of seconds
    sleep(arg);

    return 0;
}
int call_wait(int arg) {
    int status;

    // if you aren't the right pid, wait until that pid is completed
    if (waitpid(arg, &status, 0) < 0) {
        perror("Wait error: ");
    }

    return 0;
}
int call_command(char **args, int num_args) {
    
    // check for background flag
    // if it's found, make sure it's the last input
    int amp_flag = 0;

    int amp_index = 0;
    while (args[amp_index] != NULL) {
        if (args[amp_index][0] == '&') {
            // make sure its a single character and that the next position is null
            if (args[amp_index + 1] != NULL) {
                printf("Input error: '&' argument must be at end of line.\n");
                return -1;
            }
            amp_flag = 1;
        }
        amp_index++;
    }

    // also need to parse any args for piping
    // iterate through args
    // if first char of argument is < or >
    // take following chars as input or output file (check if null)
    int i = 0;
    char *input_file = NULL;
    char *output_file = NULL;
    while (args[i] != NULL) {
        if (args[i][0] == '<' && args[i][1] != '\0') {
            // ignore first character
            input_file = args[i] + 1;
        }
        else if (args[i][0] == '>' && args[i][1] != '\0') {
            output_file = args[i] + 1;
        }
        i++;
    }

    // create input and output file descriptions
    int fd_in = -1;
    int fd_out = -1;
    if (input_file != NULL) {
        if ((fd_in = open(input_file, O_RDONLY)) < 0) {
            perror("Input file error: ");
            return -1;
        }
    }
    if (output_file != NULL) {
        if ((fd_out = open(output_file, O_WRONLY | O_CREAT, 0666)) < 0) {
            perror("Output file error: ");
        }
    }
    
    // need to fork a process and add it to the process control block
    int child_pid = fork();
    if (child_pid < 0) {
        perror("Fork problem: ");
        _exit(1);
    }
    else if (child_pid == 0) {    
        // dup input to stdin
        if (fd_in != -1) {
            dup2(fd_in, STDIN_FILENO);
            close(fd_in);
        }
        // dup output to stdout
        if (fd_out != -1) {
            dup2(fd_out, STDOUT_FILENO);
            close(fd_out);
        }

        if (execvp(args[0], args) < 0) {
            perror("Exec problem: ");
            _exit(1);
        }
    }

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
    processes[process_index] = process;
    processes[process_index+1] = NULL;

    // wait until child process is done if amp not specified
    if (!amp_flag) {
        wait(NULL);
    }

    return 0;
}

// signal handler
void sigchld_handler(int sig, siginfo_t *info, void *context) {
    // https://stackoverflow.com/questions/2595503/determine-pid-of-terminated-process

    pid_t pid;
    int status;

    // handle background and non-terminated child processes
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) > 0) {
        if (WIFEXITED(status) || WIFSIGNALED(status)) {
            remove_process(pid);
        }
        else if (WIFSTOPPED(status)) {
            update_process_status(pid, 'S');
        }
        else if (WIFCONTINUED(status)) {
            update_process_status(pid, 'R');
        }
    }

    // handle already dead foreground processes
    // remove process won't do anything on an already removed pid
    // could be an issue if multiple zombie processes are waited at once in call_command and havent removed
    if ((pid = waitpid(info->si_pid, &status, WNOHANG)) < 0) {
        remove_process(info->si_pid);
    }
}