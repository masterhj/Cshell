#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

static char **tokenize(char *input, int *token_count) {
    char **tokens = malloc(MAX_TOKENS * sizeof(char *));
    *token_count = 0;
    
    char *copy = malloc(strlen(input) + 1);
    strcpy(copy, input);
    
    char *saveptr;
    char *token = strtok_r(copy, " \t\n", &saveptr);
    
    while (token && *token_count < MAX_TOKENS) {
        tokens[*token_count] = malloc(strlen(token) + 1);
        strcpy(tokens[*token_count], token);
        (*token_count)++;
        token = strtok_r(NULL, " \t\n", &saveptr);
    }
    
    free(copy);
    return tokens;
}

static void free_tokens(char **tokens, int count) {
    for (int i = 0; i < count; i++) {
        free(tokens[i]);
    }
    free(tokens);
}

static Command parse_single_command(char **tokens, int *token_idx, int total_tokens) {
    Command cmd = {0};
    cmd.args = malloc(MAX_ARGS * sizeof(char *));
    cmd.argc = 0;
    cmd.append = 0;
    cmd.input_file = NULL;
    cmd.output_file = NULL;
    
    while (*token_idx < total_tokens) {
        char *token = tokens[*token_idx];
        
        if (strcmp(token, "|") == 0) {
            break;
        }
        
        if (strcmp(token, "<") == 0) {
            (*token_idx)++;
            if (*token_idx < total_tokens) {
                cmd.input_file = malloc(strlen(tokens[*token_idx]) + 1);
                strcpy(cmd.input_file, tokens[*token_idx]);
            }
            (*token_idx)++;
            continue;
        }
        
        if (strcmp(token, ">") == 0) {
            (*token_idx)++;
            if (*token_idx < total_tokens) {
                cmd.output_file = malloc(strlen(tokens[*token_idx]) + 1);
                strcpy(cmd.output_file, tokens[*token_idx]);
                cmd.append = 0;
            }
            (*token_idx)++;
            continue;
        }
        
        if (strcmp(token, ">>") == 0) {
            (*token_idx)++;
            if (*token_idx < total_tokens) {
                cmd.output_file = malloc(strlen(tokens[*token_idx]) + 1);
                strcpy(cmd.output_file, tokens[*token_idx]);
                cmd.append = 1;
            }
            (*token_idx)++;
            continue;
        }
        
        if (cmd.argc == 0) {
            cmd.cmd = malloc(strlen(token) + 1);
            strcpy(cmd.cmd, token);
        }
        
        cmd.args[cmd.argc] = malloc(strlen(token) + 1);
        strcpy(cmd.args[cmd.argc], token);
        cmd.argc++;
        
        (*token_idx)++;
    }
    
    cmd.args[cmd.argc] = NULL;
    
    return cmd;
}

Pipeline *parse_input(char *input) {
    if (!input || strlen(input) == 0) {
        return NULL;
    }
    
    int token_count = 0;
    char **tokens = tokenize(input, &token_count);
    
    if (token_count == 0) {
        free_tokens(tokens, token_count);
        return NULL;
    }
    
    Pipeline *pipeline = malloc(sizeof(Pipeline));
    pipeline->commands = malloc(MAX_ARGS * sizeof(Command));
    pipeline->count = 0;
    
    int token_idx = 0;
    
    while (token_idx < token_count) {
        Command cmd = parse_single_command(tokens, &token_idx, token_count);
        
        if (cmd.cmd) {
            pipeline->commands[pipeline->count] = cmd;
            pipeline->count++;
        }
        
        if (token_idx < token_count && strcmp(tokens[token_idx], "|") == 0) {
            token_idx++;
        }
    }
    
    free_tokens(tokens, token_count);
    
    if (pipeline->count == 0) {
        free(pipeline->commands);
        free(pipeline);
        return NULL;
    }
    
    return pipeline;
}

void free_pipeline(Pipeline *pipeline) {
    if (!pipeline) return;
    
    for (int i = 0; i < pipeline->count; i++) {
        free(pipeline->commands[i].cmd);
        
        for (int j = 0; j < pipeline->commands[i].argc; j++) {
            free(pipeline->commands[i].args[j]);
        }
        free(pipeline->commands[i].args);
        
        if (pipeline->commands[i].input_file) {
            free(pipeline->commands[i].input_file);
        }
        if (pipeline->commands[i].output_file) {
            free(pipeline->commands[i].output_file);
        }
    }
    
    free(pipeline->commands);
    free(pipeline);
}
