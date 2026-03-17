#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "builtins.h"

extern char **environ;
extern char **history;
extern int history_count;

int is_builtin(const char *cmd) {
    if (!cmd) return 0;
    
    return strcmp(cmd, "cd") == 0 ||
           strcmp(cmd, "exit") == 0 ||
           strcmp(cmd, "help") == 0 ||
           strcmp(cmd, "pwd") == 0 ||
           strcmp(cmd, "history") == 0;
}

static int builtin_cd(char **args, int argc) {
    if (argc == 1) {
        const char *home = getenv("HOME");
        if (home && chdir(home) == 0) return 0;
        fprintf(stderr, "cd: could not change to home directory\n");
        return 1;
    }
    
    const char *path = args[1];
    const char *home = getenv("HOME");
    
    if (strcmp(path, "~") == 0) {
        if (home && chdir(home) == 0) return 0;
        fprintf(stderr, "cd: could not change to home directory\n");
        return 1;
    }
    
    if (chdir(path) == 0) {
        return 0;
    }
    
    fprintf(stderr, "cd: %s: No such file or directory\n", path);
    return 1;
}

static int builtin_pwd(char **args __attribute__((unused)), int argc __attribute__((unused))) {
    char cwd[4096];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("%s\n", cwd);
        return 0;
    }
    
    fprintf(stderr, "pwd: error reading current directory\n");
    return 1;
}

static int builtin_help(char **args __attribute__((unused)), int argc __attribute__((unused))) {
    printf("cshell - Minimal Unix Shell\n\n");
    printf("Built-in commands:\n");
    printf("  cd [dir]     - Change directory\n");
    printf("  pwd          - Print working directory\n");
    printf("  help         - Show this help message\n");
    printf("  history      - Show command history\n");
    printf("  exit         - Exit the shell\n\n");
    printf("Features:\n");
    printf("  |            - Pipe output between commands\n");
    printf("  >            - Redirect output to file\n");
    printf("  >>           - Append output to file\n");
    printf("  <            - Redirect input from file\n");
    printf("  Ctrl+C       - Does not kill shell\n");
    return 0;
}

static int builtin_history(char **args __attribute__((unused)), int argc __attribute__((unused))) {
    for (int i = 0; i < history_count; i++) {
        printf("%3d  %s\n", i + 1, history[i]);
    }
    return 0;
}

int execute_builtin(char *cmd, char **args, int argc) {
    if (strcmp(cmd, "cd") == 0) {
        return builtin_cd(args, argc);
    }
    if (strcmp(cmd, "pwd") == 0) {
        return builtin_pwd(args, argc);
    }
    if (strcmp(cmd, "help") == 0) {
        return builtin_help(args, argc);
    }
    if (strcmp(cmd, "history") == 0) {
        return builtin_history(args, argc);
    }
    if (strcmp(cmd, "exit") == 0) {
        exit(0);
    }
    
    return 1;
}
