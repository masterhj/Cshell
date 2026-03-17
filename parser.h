#ifndef PARSER_H
#define PARSER_H

#define MAX_ARGS 64
#define MAX_CMD_LEN 1024
#define MAX_TOKENS 128

typedef struct {
    char *cmd;
    char **args;
    int argc;
    char *input_file;
    char *output_file;
    int append;
} Command;

typedef struct {
    Command *commands;
    int count;
} Pipeline;

Pipeline *parse_input(char *input);
void free_pipeline(Pipeline *pipeline);

#endif
