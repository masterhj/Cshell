# cshell - Minimal Unix Shell

A minimal Unix shell implementation in C with support for pipes, redirections, built-in commands, and command history.

## Features

- **External Commands**: Execute any external command by forking child processes
- **Built-in Commands**:
  - `cd [dir]` - Change directory (default: home directory)
  - `pwd` - Print working directory
  - `help` - Display help message with all features
  - `history` - Show command history (last 256 commands)
  - `exit` - Exit the shell

- **Piping**: Chain multiple commands with `|`
  - Example: `ls | grep .c | wc -l`

- **Input/Output Redirection**:
  - `<` - Redirect input from file
  - `>` - Redirect output to file (truncate)
  - `>>` - Append output to file
  - Example: `echo "hello" > file.txt`

- **Signal Handling**: Ctrl+C does not kill the shell, only the current command

- **Command History**: All commands are stored and accessible via the `history` command

## Build Instructions

### Prerequisites
- GCC compiler
- Standard C library (libc)
- POSIX-compliant operating system (Linux, macOS, BSD)

### Building

```bash
cd /path/to/cshell
make
```

This will compile all source files and create the `cshell` executable.

### Running

```bash
./cshell
```

Or use the convenience target:

```bash
make run
```

### Cleaning

Remove compiled files:

```bash
make clean
```

## Usage Examples

```bash
cshell> pwd
/Users/himanshujaiswal/Documents/cshell

cshell> cd ~
cshell> ls

cshell> cat file.txt | grep "search" | wc -l

cshell> echo "test" > output.txt

cshell> cat < input.txt

cshell> history

cshell> help

cshell> exit
```

## Project Structure

```
cshell/
├── main.c          - Main event loop, signal handling, history management
├── parser.c        - Input parsing, tokenization, command structure building
├── parser.h        - Parser interface and structures
├── executor.c      - Command execution, piping, fork/exec handling
├── executor.h      - Executor interface
├── builtins.c      - Built-in command implementations
├── builtins.h      - Built-in command interface
├── Makefile        - Build configuration
├── README.md       - This file
└── CHANGELOG.md    - Version history
```

## Implementation Details

### Parser
- Tokenizes input by whitespace
- Handles pipes (`|`) to identify command boundaries
- Handles redirections (`<`, `>`, `>>`) with associated filenames
- Returns a pipeline structure with individual commands

### Executor
- Handles single commands, built-in or external
- Creates pipes between commands in a pipeline
- Forks child processes for external commands using `fork()` and `execvp()`
- Handles input/output redirections via file descriptors and `dup2()`
- Waits for all child processes to complete

### Built-ins
- `cd`: Uses `chdir()` to change directories
- `pwd`: Uses `getcwd()` to get current directory
- `help`: Displays usage information
- `history`: Shows all stored commands
- `exit`: Terminates the shell

### Main Loop
- Initializes signal handler for SIGINT
- Maintains command history (max 256 entries)
- Reads user input and calls parser/executor
- Continues on signal interruption

## Limitations

- Piping only works with external commands (built-ins cannot be piped)
- No background process support (no `&` operator)
- No job control (no `fg`, `bg`, `jobs` commands)
- No environment variable expansion
- No quoting or escape sequences
- No aliases or command substitution
- Limited to 64 arguments per command
- Limited to 128 tokens per input line

## Technical Notes

- Uses only POSIX APIs and standard C library
- No external dependencies
- Handles process termination and cleanup properly
- Safe memory management with malloc/free
- Robust error handling and reporting

## License

Public domain. Free to use and modify.
