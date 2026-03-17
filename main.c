#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "parser.h"
#include "executor.h"
#include "builtins.h"

#define MAX_HISTORY 256
#define MAX_INPUT_LEN 1024

char **history;
int history_count = 0;

static void init_history() {
    history = malloc(MAX_HISTORY * sizeof(char *));
    for (int i = 0; i < MAX_HISTORY; i++) {
        history[i] = NULL;
    }
}

static void add_to_history(const char *cmd) {
    if (history_count >= MAX_HISTORY) {
        free(history[0]);
        for (int i = 0; i < MAX_HISTORY - 1; i++) {
            history[i] = history[i + 1];
        }
        history_count = MAX_HISTORY - 1;
    }
    
    history[history_count] = malloc(strlen(cmd) + 1);
    strcpy(history[history_count], cmd);
    history_count++;
}

static void cleanup_history() {
    for (int i = 0; i < history_count; i++) {
        free(history[i]);
    }
    free(history);
}

static void handle_sigint(int sig __attribute__((unused))) {
    printf("\n");
    fflush(stdout);
}

static void print_prompt() {
    printf("cshell> ");
    fflush(stdout);
}

int main() {
    init_history();
    
    signal(SIGINT, handle_sigint);
    
    char input[MAX_INPUT_LEN];
    
    while (1) {
        print_prompt();
        
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("\n");
            break;
        }
        
        size_t len = strlen(input);
        if (len > 0 && input[len - 1] == '\n') {
            input[len - 1] = '\0';
        }
        
        if (strlen(input) == 0) {
            continue;
        }
        
        add_to_history(input);
        
        Pipeline *pipeline = parse_input(input);
        
        if (pipeline) {
            execute_pipeline(pipeline);
            free_pipeline(pipeline);
        }
    }
    
    cleanup_history();
    
    return 0;
}
