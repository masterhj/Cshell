# Changelog

All notable changes to the cshell project are documented in this file.

## [1.0.0] - 2026-03-18

### Added
- Initial release of cshell
- Support for running external commands via fork/exec
- Built-in commands: cd, pwd, help, history, exit
- Pipe operator (|) for chaining commands
- Input redirection with < operator
- Output redirection with > and >> operators
- Signal handling for SIGINT (Ctrl+C) without terminating shell
- Command history with last 256 commands stored
- Modular architecture with separate parser, executor, and builtins modules
- Makefile for easy compilation
- Comprehensive README with usage examples
- No external dependencies - pure POSIX C implementation
