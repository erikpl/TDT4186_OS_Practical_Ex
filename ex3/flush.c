#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>

#define BUFFER_LENGTH 700
#define INVALID_REDIRECT -2
#define NO_REDIRECT -1


// General global variables
int status;  
char *cmnd;
char cwd[BUFFER_LENGTH];

// Arguments variables
char *arguments[BUFFER_LENGTH];
int arg_idx = 0;

// Built in command variables
char *built_in_cmd [] = {"cd", "jobs"};
int built_in_cmd_len = sizeof(built_in_cmd)/sizeof(built_in_cmd[0]);

// Hold the new program after redirections and new locations have been removed.
char *program_after_io[BUFFER_LENGTH];


// Returnerer current working directory
char * get_current_directory() {
    getcwd(cwd, sizeof(cwd));
    return cwd;
}

void set_subarray_from_args(int start_index, int stop_index, char **array[BUFFER_LENGTH]) {
    int array_size = stop_index - start_index + 1;
    for (int i = 0; i < array_size; i++) {
        *array[i] = arguments[i];
    }
}

void emtpy_args () {
    memset(arguments, 0, BUFFER_LENGTH * sizeof(arguments[0]));
    memset(program_after_io, 0, BUFFER_LENGTH * sizeof(arguments[0]));
}

// Håndterer brukerinput og legger kommandoen (første string) til i cmnd, og argumentene i arguments
void handle_user_input(char input[BUFFER_LENGTH]) {
    // Splitter på mellomrom og tab
    char split_on[3] = {' ', '\t', '\0'};
    char *word = strtok(input, split_on);

    // Lagrer første string i den globale variablen cmnd
    cmnd = word;

    // Lagrer argumentene i arguments
    while (word != NULL) {
        arguments[arg_idx] = word;
        word = strtok(NULL, split_on);
        arg_idx++;
    }
    arg_idx = 0;
}

void execute_built_in() {

    // If the command is cd, change directory with built in chdir function
    if(strcmp(cmnd, "cd") == 0) {
        printf("Arguments = %s\n", arguments[1]);
        if(arguments[0] == NULL) {
            printf("The path is empty.\n");
        }

        chdir(arguments[1]);
    }  
}

void get_io_type(int *output_index, int *input_index) {
    printf("Executing io type\n");
    int input_redir_pos = -1;
    int output_redir_pos = -1;

    int loop_counter = 0;
    while (arguments[loop_counter] != 0) {
        printf("Argument nr. %d: %s\n", loop_counter, arguments[loop_counter]);
        printf("strcmp(echo, >): %d\n", strcmp("echo", ">"));
        if (strcmp(arguments[loop_counter], ">") == 0) {
            printf("Found output\n");
            if (output_redir_pos != -1) {
                // More than one output redirect is not supported
                *output_index = INVALID_REDIRECT;
                *input_index = INVALID_REDIRECT;
                break;
            }

            output_redir_pos = loop_counter;
        }

        if (strcmp(arguments[loop_counter], "<") == 0) {
            printf("Found input\n");
            if (input_redir_pos != -1) {
                // More than one input redirect is not supported
                *output_index = INVALID_REDIRECT;
                *input_index = INVALID_REDIRECT;
                break;
            }
            input_redir_pos = loop_counter;
        }

        if (output_redir_pos < input_redir_pos) {
            // Redirecting output before input is not supported
            *output_index = INVALID_REDIRECT;
            *input_index = INVALID_REDIRECT;
            break;
        }

        loop_counter++;
    }

    // Check if no redirect
    printf("Input redir pos: %d\n", input_redir_pos);
    printf("Output redir pos: %d\n", output_redir_pos);
    if (input_redir_pos == -1 && output_redir_pos == -1) {
        printf("No redirect");
        *input_index = NO_REDIRECT;
        *output_index = NO_REDIRECT;
    }
    else {
        printf("Yes redirect\n");
        *input_index = input_redir_pos;
        *output_index = output_redir_pos;
    }
}

void execute_bin() {

    // Get PIDs
    pid_t parent_pid = getpid();
    pid_t child_pid = fork();
    pid_t curr_pid = getpid();
    
    if (curr_pid == -1 && child_pid == 0) {
        printf("\nThere was an error when trying to fork.\n");
        return;
    }

    // For the parent process, print the status of the child process execution
    else if (child_pid > 0) {
        if(waitpid(child_pid, &status, 0) == -1) {
            printf("Error: Failed calling waitpid.\n");
            exit(EXIT_FAILURE);
        }
        
        if(WIFEXITED(status)) {
            int exit_status = WEXITSTATUS(status);     
            printf("\nExit status [%s %s] = %d\n", cmnd, arguments[1], exit_status);
        }
    }

    // Child_pid is 0 when we are in the child process. Run command
    else if (child_pid == 0) {

        int input_redir_pos;
        int output_redir_pos;
        // Sets redirect indices. -1 for no redirect, -2 for invalid redirect.
        get_io_type(&input_redir_pos, &output_redir_pos);

        // Check if the command has invalid redirections
        if (input_redir_pos == -2 || output_redir_pos == -2) {
            fprintf(stderr, "Invalid redirection syntax.");
            return;
        }

        // If there is a valid output redirection
        if (output_redir_pos != NO_REDIRECT) {
            // Replae STDOUT with the file (located at arguments[output_redir_pos + 1]).
            // "w" denotes that the file will be overwritten and created if it doesn't exist.
            freopen(arguments[output_redir_pos + 1], "w", stdout);
            // TODO: will necessitate either changing the arguments array or executing it differently
        }

        // If there is a valid output redirection
        if (input_redir_pos != NO_REDIRECT) {
            // Replae STDIN with the file (located at arguments[input_redir_pos + 1]).
            // "r" denotes that the file will simply be read.
            freopen(arguments[input_redir_pos + 1], "r", stdin);
        }

        int has_redirect = output_redir_pos != -1 || input_redir_pos != -1;
        // If no redirect, execute the command directly
        if (has_redirect) {
            execvp(cmnd, arguments);
        }
        // If there is a redirect, the executed command must be extracted
        else {
            int first_delimiter = (output_redir_pos > input_redir_pos) ? input_redir_pos : output_redir_pos;
            set_subarray_from_args(0, first_delimiter - 1, &program_after_io);
            execvp(cmnd, program_after_io);
        }

        // Return error from command-execution
        printf("There was an error: %s\n", strerror(errno));
        exit(0);
    }

    // Waiting for child process to complete
    else {
        wait(NULL);
        return;
    }
}

int main() {

    char user_input[BUFFER_LENGTH];
    int is_executed = 0;
    char * line[BUFFER_LENGTH];

    while (1) {
        // Get user input
        printf("\n%s: ", get_current_directory());
        fgets(user_input, BUFFER_LENGTH, stdin);
        fflush(stdin);
        printf("\n");
        

        // Fjerner mellomrommet på slutten av user_input
        user_input[strlen(user_input) - 1] = '\0';

        handle_user_input(user_input);


        // If user presses ctrl-d and thus no inputs are given.
        if (arguments[0] == NULL ) {
            printf("Exiting flush.");
            return 0;
        }

        // Check if the command is built into the shell
        for(int i = 0; i < built_in_cmd_len; ++i) {

            // Execute built in command
            if (!strcmp(built_in_cmd[i], cmnd) && !is_executed) {
                execute_built_in();
                is_executed = 1;
            }
        }

        // The command was not built in. Run command with path-matching.
        if (!is_executed) {
            execute_bin();
            is_executed = 1;
        }

        // Clean arguments array
        emtpy_args();
        is_executed = 0;
    }
}