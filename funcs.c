/***************************************************************
 *
 * file: funcs.c
 *
 * @author Nikos Lefakis csd4804

 *
 * @e-mail       csd4804@csd.uoc.gr
 *
 * @brief  Implement function of Library Funcs.h 
 * with handling command , pipes and redirection for a simple UNIX shell in C
 *
 ***************************************************************
 */


#include "funcs.h"


void start_shell(void){
    printf("\033[H\033[J");
    printf("\n\n\n\n******************"
        "************************"); 
    printf("\n\n\n\t****WELCOME TO MY SHELL IN CS-345****"); 
    printf("\n\n\t****ENJOY , WITH OWN RISK!****"); 
    printf("\n\n\n\n*******************"
        "***********************"); 
  
    printf("\n"); 
    sleep(2); 
    printf("\033[H\033[J");
}

//prompt function to show user and directory
void command_prompt(void){
    printf("\033[1;32mcsd4804-hy345sh@%s\033[0m: \033[1;34m%s$\033[0m $\033[0m ", get_current_user(), get_directory_user());
    return;
}


// take current directory
char* get_directory_user(void){

    char* cwd = (char *) malloc(PATH_MAX);
    check_mem(cwd);
    
    if (getcwd(cwd, PATH_MAX) == NULL) {
        
        return NULL;
    }
   
    return cwd;
}

//get current user that use the shell 
char* get_current_user(void){
     __uid_t uid = getuid();
    struct passwd *pw = getpwuid(uid);
    if(pw == NULL) return NULL; 

    return pw->pw_name;

}

//take input from stdin (user command)
char* read_command(void) {
    char *command = NULL;
    size_t len = 0;

    ssize_t cmd = getline(&command, &len, stdin);

    if (cmd < 0 ) {
        printf("Error reading command.\n");
        return NULL;
    }

    // Remove newline character if present
    if (command[cmd - 1] == '\n')
        command[cmd - 1] = '\0';

    return command;
}

//split commands with semicolon or redirection handling
void split_commands(char *input, Command_t *cmd) {
    if (input == NULL)
        return;
    if(strcmp(input,"quit") == 0 ){
        printf("\033[1;31mExiting.....\033[0m \n");
        exit(EXIT_SUCCESS);
    }
         
    char *token = strtok(input, ";");
    int i = 0;
    while (token != NULL) {
            // Check for redirection symbols
        char *redirect_input = strstr(token, "<");
        char *redirect_output = strstr(token, ">");
        char *append_output = strstr(token, ">>");

        if (redirect_input != NULL || redirect_output != NULL || append_output != NULL) {
            // Execute the command with redirection
            
            execute_commands(token);
        } else {
            cmd->commands[i] = token;
            i++;
        }

        token = strtok(NULL, ";");
        }

    cmd->commands[i] = NULL;
}



// fork and execute according to pid
void execArgs(Command_t* cmd){
   
    
    for (int i = 0; cmd->commands[i] != NULL; i++) {
     
        pid_t pid = fork();
        if (pid < 0 ) {
            perror("Failed forking child");
            exit(EXIT_FAILURE);
        } else if (pid == 0){
            execute_commands(cmd->commands[i]);
            exit(EXIT_SUCCESS);
        } else {
             // Parent process
            int status;
            waitpid(pid, &status, 0);
        }
    }
        
}    

//Execute the command that provide the user
void execute_commands(char *cmd) {
    
    // Check for redirection symbols 
    char *redirect_input = strstr(cmd, "<");
    char *redirect_output = strstr(cmd, ">");
    char *append_output = strstr(cmd, ">>");

    int input_redirection = 0;  // 0: no input redirection, 1: input redirection
    int output_redirection = 0; // 0: no output redirection, 1: output redirection, 2: append redirection
    char *input_file = NULL;
    char *output_file = NULL;

    if (redirect_input != NULL) {
        input_redirection = 1;
        input_file = strtok(redirect_input + 1, " ");
    }

    if (redirect_output != NULL) {
        if (append_output != NULL) {
            output_redirection = 2;
            output_file = strtok(append_output + 2, " ");
        } else {
            output_redirection = 1;
            output_file = strtok(redirect_output + 1, " ");
        }
    }

    pid_t pid = fork();

    if (pid < 0) {
        perror("Failed forking child");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Child process
        if (input_redirection) {
            int input_fd = open(input_file, O_RDONLY);
            if (input_fd < 0) {
                perror("Input redirection failed");
                exit(EXIT_FAILURE);
            }
            dup2(input_fd, STDIN_FILENO);
            close(input_fd);
        }

        if (output_redirection == 1) {
            int output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if (output_fd < 0) {
                perror("Output redirection failed");
                exit(EXIT_FAILURE);
            }
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        } else if (output_redirection == 2) {
            int output_fd = open(output_file, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
            if (output_fd < 0) {
                perror("Append redirection failed");
                exit(EXIT_FAILURE);
            }
            dup2(output_fd, STDOUT_FILENO);
            close(output_fd);
        }

        // Execute the command without redirection
        char **args = split_args(cmd);
      
        if (execvp(args[0], args) < 0) {
            perror("Execution failed");
            exit(EXIT_FAILURE);
         }
        // This should not be reached, but just in case
        exit(EXIT_SUCCESS);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);
    }
 }

    

char **split_args(char *cmd) {
    char **args = (char **)malloc((PATH_MAX + 1) * sizeof(char *));
    check_mem(args);

    char *delimiters = " ";
    char *token = strtok(cmd, delimiters);
    int i = 0;
    while (token != NULL) {
        if (strcmp(token, "<") == 0 || strcmp(token, ">") == 0 || strcmp(token, ">>") == 0) {
            // Handle redirection symbols
            args[i] = NULL;  
            i++;
            args[i] = (char *)malloc(strlen(token) + 1);
            check_mem(args[i]);
            strcpy(args[i], token);
            i++;
        } else {
            args[i] = (char *)malloc(strlen(token) + 1);
            check_mem(args[i]);
            strcpy(args[i], token);
            i++;
        }
        token = strtok(NULL, delimiters);
    }
    args[i] = NULL;

    return args;
}







//split pipes command line appropriately
void split_pipes(char* input ,Command_t* cmd){
    if(input == NULL) return;
    char* pipe_cmd = strtok(input,"|");
    int count = 0;
    while(pipe_cmd != NULL){
        cmd->commands[count] = pipe_cmd;
        count++;
        pipe_cmd = strtok(NULL,"|");
    }
    cmd->commands[count] = NULL;
    cmd->number_of_pipes = count - 1;
}


void execute_pipes(Command_t* cmd) {
    int num_pipes = cmd->number_of_pipes;
    int pipe_fd[num_pipes][2];  // Adjusted to exclude the last pipe

    for (int i = 0; i < num_pipes; i++) {
        pipe(pipe_fd[i]);
    }

    for (int j = 0; cmd->commands[j] != NULL; j++) {
        pid_t pid = fork();

        if (pid < 0) {
            perror("Failed to fork child!");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Child process

            // Handle input redirection for the first command
            if (j == 0) {
                char *redirect_input = strstr(cmd->commands[j], "<");
                if (redirect_input != NULL) {
                    char *input_file = strtok(redirect_input + 1, " ");
                    int input_fd = open(input_file, O_RDONLY);
                    if (input_fd < 0) {
                        perror("Input redirection failed");
                        exit(EXIT_FAILURE);
                    }
                    dup2(input_fd, STDIN_FILENO);
                    close(input_fd);
                }
            }

            // Handle output redirection for the last command
            if (j == num_pipes) {
                char *redirect_output = strstr(cmd->commands[j], ">");
                char *append_output = strstr(cmd->commands[j], ">>");

                if (redirect_output != NULL) {
                    char *output_file = strtok(redirect_output + 1, " ");
                    int output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                    if (output_fd < 0) {
                        perror("Output redirection failed");
                        exit(EXIT_FAILURE);
                    }
                    dup2(output_fd, STDOUT_FILENO);
                    close(output_fd);
                } else if (append_output != NULL) {
                    char *output_file = strtok(append_output + 2, " ");
                    int output_fd = open(output_file, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
                    if (output_fd < 0) {
                        perror("Append redirection failed");
                        exit(EXIT_FAILURE);
                    }
                    dup2(output_fd, STDOUT_FILENO);
                    close(output_fd);
                }
            }

            // Connect pipes
            if (j != 0) {
                dup2(pipe_fd[j-1][0], STDIN_FILENO);
            }
            if (j != num_pipes) {
                dup2(pipe_fd[j][1], STDOUT_FILENO);
            }

            for (int k = 0; k < num_pipes; k++) {
                close(pipe_fd[k][0]);
                close(pipe_fd[k][1]);
            }

            char **args = split_args(cmd->commands[j]);    
            if (execvp(args[0], args) < 0) {
                perror("Execution failed");    //execute pipes 
                exit(EXIT_FAILURE);
            }
        }
    }

    for (int i = 0; i < num_pipes; i++) {
        close(pipe_fd[i][0]);
        close(pipe_fd[i][1]);
    }

    for (int i = 0; i < num_pipes + 1; i++) {
        wait(NULL);  // Wait for all child processes to finish
    }
}


//free args 
void free_arguments(char **args) {
    if (args != NULL) {
        for (int i = 0; args[i] != NULL; i++) {
            free(args[i]);
        }
        free(args);
    }
}


void cd_command(char *directory) {
    if (directory == NULL || strlen(directory) == 0) {
        //if cd command is only cd then redirect to home directory
        char *home_dir = getenv("HOME");
        if (home_dir == NULL) {
            printf("cd: Error with home directory\n");
            return;
        }
        if (chdir(home_dir) != 0) {
            perror("cd command failed");
        }
    } else {
        if (chdir(directory) != 0) {
            perror("cd command failed");
        }
    }
}


// checking memory  errors
void check_mem(void* ptr){
    if(ptr == NULL){
        perror("Memory Allocation failed! \n");
        exit(EXIT_FAILURE);
    }
}


//I make this function because I have segmentation fault when I type a cd command
char *trim_spaces(char *str) {
    // Remove leading spaces
    while (*str && isspace(*str)) {
        str++;
    }

    // Remove  spaces from end
    char *end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) {
        *end-- = '\0';
    }

    return str;
}
