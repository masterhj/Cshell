#ifndef BUILTINS_H
#define BUILTINS_H

int is_builtin(const char *cmd);
int execute_builtin(char *cmd, char **args, int argc);

#endif
