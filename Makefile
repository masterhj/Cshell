CC = gcc
CFLAGS = -Wall -Wextra -Werror -O2
SRCS = main.c parser.c executor.c builtins.c
OBJS = $(SRCS:.c=.o)
TARGET = cshell

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all run clean
