/***************************************************************
 *
 * file: shell.c
 *
 * @author Nikos Lefakis csd4804

 *
 * @e-mail       csd4804@csd.uoc.gr
 *
 * @brief  Implement main function of shell with prompt of user and 
 * according to user's input, the shell will execute the corresponding 
 *
 ***************************************************************
 */

#include "funcs.h"



int main(int argc, char *argv[]) {
    
    start_shell();

    while (1) {
        command_prompt();
        Command_t *token = (Command_t *)malloc(sizeof(Command_t));
        check_mem(token);
        char *cmd = read_command();  //take input from user

 
        char *trimmed_cmd = trim_spaces(cmd);

        if (strncmp(trimmed_cmd, "chdir", 5) == 0) {
            // Handle the cd command
            char *dir = trimmed_cmd + 5;  // Skip cd and go to directoy
           
            char *t_dir = trim_spaces(dir);
            cd_command(t_dir);
        } else {
            // Check if the command is a pipeline
            if (strchr(trimmed_cmd, '|') != NULL) {
                split_pipes(trimmed_cmd, token);
                execute_pipes(token);
            } else {
                // Handle other commands
                split_commands(trimmed_cmd, token);
                execArgs(token);
            }
        }

        free(token);
        free(trimmed_cmd);  // Free the trimmed command

        // Free arguments if needed
    }

    return EXIT_SUCCESS;
}

