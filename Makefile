#######################################
#
#	HY-345
#
# file: Makefile
#	
# @Author: Nikos Lefakis 4804
# @Version: GNU Make 4.2.1
#
#
#######################################

CC = gcc
CFLAGS = -Wall -g
SRC = shell.c funcs.c
OBJ = $(SRC:.c=.o)
TARGET = hy345sh

all: $(TARGET)
	rm -f $(OBJ)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET)

.PHONY: all clean
