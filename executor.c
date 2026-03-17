#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "executor.h"
#include "builtins.h"

static int handle_redirections(Command *cmd) {
    if (cmd->input_file) {
        int fd = open(cmd->input_file, O_RDONLY);
        if (fd == -1) {
            fprintf(stderr, "cshell: cannot open input file '%s'\n", cmd->input_file);
            return -1;
        }
        if (dup2(fd, STDIN_FILENO) == -1) {
            fprintf(stderr, "cshell: dup2 failed for input\n");
            close(fd);
            return -1;
        }
        close(fd);
    }
    
    if (cmd->output_file) {
        int flags = O_WRONLY | O_CREAT;
        if (cmd->append) {
            flags |= O_APPEND;
        } else {
            flags |= O_TRUNC;
        }
        
        int fd = open(cmd->output_file, flags, 0644);
        if (fd == -1) {
            fprintf(stderr, "cshell: cannot open output file '%s'\n", cmd->output_file);
            return -1;
        }
        if (dup2(fd, STDOUT_FILENO) == -1) {
            fprintf(stderr, "cshell: dup2 failed for output\n");
            close(fd);
            return -1;
        }
        close(fd);
    }
    
    return 0;
}

static int execute_single_command(Command *cmd, int read_fd, int write_fd) {
    if (is_builtin(cmd->cmd)) {
        if (write_fd != STDOUT_FILENO) {
            dup2(write_fd, STDOUT_FILENO);
            close(write_fd);
        }
        if (read_fd != STDIN_FILENO) {
            dup2(read_fd, STDIN_FILENO);
            close(read_fd);
        }
        
        return execute_builtin(cmd->cmd, cmd->args, cmd->argc);
    }
    
    pid_t pid = fork();
    
    if (pid == -1) {
        fprintf(stderr, "cshell: fork failed\n");
        return -1;
    }
    
    if (pid == 0) {
        if (read_fd != STDIN_FILENO) {
            dup2(read_fd, STDIN_FILENO);
            close(read_fd);
        }
        if (write_fd != STDOUT_FILENO) {
            dup2(write_fd, STDOUT_FILENO);
            close(write_fd);
        }
        
        if (handle_redirections(cmd) == -1) {
            exit(1);
        }
        
        execvp(cmd->cmd, cmd->args);
        fprintf(stderr, "cshell: command not found: %s\n", cmd->cmd);
        exit(127);
    }
    
    if (read_fd != STDIN_FILENO) close(read_fd);
    if (write_fd != STDOUT_FILENO) close(write_fd);
    
    int status;
    waitpid(pid, &status, 0);
    
    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    }
    
    return -1;
}

int execute_pipeline(Pipeline *pipeline) {
    if (!pipeline || pipeline->count == 0) {
        return 0;
    }
    
    if (pipeline->count == 1) {
        return execute_single_command(&pipeline->commands[0], STDIN_FILENO, STDOUT_FILENO);
    }
    
    int pipes[pipeline->count - 1][2];
    
    for (int i = 0; i < pipeline->count - 1; i++) {
        if (pipe(pipes[i]) == -1) {
            fprintf(stderr, "cshell: pipe failed\n");
            return -1;
        }
    }
    
    pid_t pids[pipeline->count];
    
    for (int i = 0; i < pipeline->count; i++) {
        pids[i] = fork();
        
        if (pids[i] == -1) {
            fprintf(stderr, "cshell: fork failed\n");
            return -1;
        }
        
        if (pids[i] == 0) {
            int read_fd = (i == 0) ? STDIN_FILENO : pipes[i - 1][0];
            int write_fd = (i == pipeline->count - 1) ? STDOUT_FILENO : pipes[i][1];
            
            for (int j = 0; j < pipeline->count - 1; j++) {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }
            
            if (read_fd != STDIN_FILENO) {
                dup2(read_fd, STDIN_FILENO);
                close(read_fd);
            }
            if (write_fd != STDOUT_FILENO) {
                dup2(write_fd, STDOUT_FILENO);
                close(write_fd);
            }
            
            if (handle_redirections(&pipeline->commands[i]) == -1) {
                exit(1);
            }
            
            execvp(pipeline->commands[i].cmd, pipeline->commands[i].args);
            fprintf(stderr, "cshell: command not found: %s\n", pipeline->commands[i].cmd);
            exit(127);
        }
    }
    
    for (int i = 0; i < pipeline->count - 1; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    
    int last_status = 0;
    for (int i = 0; i < pipeline->count; i++) {
        int status;
        waitpid(pids[i], &status, 0);
        if (i == pipeline->count - 1) {
            if (WIFEXITED(status)) {
                last_status = WEXITSTATUS(status);
            }
        }
    }
    
    return last_status;
}
