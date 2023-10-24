/***************************************************************
 *
 * file: funcs.h
 *
 * @author Nikos Lefakis csd4804

 *
 * @e-mail       csd4804@csd.uoc.gr
 *
 * @brief Declaration of  functions for handling a command line (Program's LIbrary)
 * 
 *
 ***************************************************************
 */



#ifndef FUNCS_H
#define FUNCS_H

#include <stdio.h>
#include <unistd.h>
#include <pwd.h>
#include <stdlib.h>
#include<sys/wait.h>
#include <string.h>
#include <fcntl.h>  
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <stdbool.h>

#define PATH_MAX 4096

typedef struct command{
    char* commands[PATH_MAX];
    int number_of_pipes;
}Command_t;

void start_shell(void);

void command_prompt(void);

char*  read_command(void);

char* get_directory_user(void);

char* get_current_user(void);

void execArgs(Command_t* cmd);

void split_commands(char* input , Command_t* cmd);

char** split_args(char* cmd);

void execute_commands(char* token);

void execute_pipes(Command_t* cmd);

void split_pipes(char* input ,Command_t* cmd);

void check_mem(void* ptr);

void free_arguments(char **args);

void cd_command(char *directory);

char *trim_spaces(char *str);

#endif