# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g -I/usr/include/tcl8.6 -ltcl8.6 -lreadline

# Source files
SRC = main.c tcl.c readline.c my_structs.c my_parse.c

# Object files
OBJ = $(SRC:.c=.o)

# Executable name
EXEC = TCL_Shell

# Main target
all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

# Compile source files into object files
%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

# Clean up object files and executable
clean:
	rm -f $(OBJ) $(EXEC)
