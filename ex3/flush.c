#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <strings.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>


#define BUFFER_LENGTH 700

#define INVALID_REDIRECT -2
#define NO_REDIRECT -1

// General global variables
int status;  
char *cmnd;
char cwd[BUFFER_LENGTH];

// Arguments variables
char *arguments[BUFFER_LENGTH];
char *program[BUFFER_LENGTH];


void set_cmd_subarray(int start_idx, int stop_idx, char **array) {
    int array_size = stop_idx - start_idx + 1;

    for (int i = 0; i < array_size; i++) {
        array[i] = arguments[i];
    }
}

/*
    Sets an array on the form [input_redirect_pos, output_redirect_pos].
    The value [-1, -1] denotes no redirect.
    The value [-2, -2] denotes that the query uses redirects which are not supported.
*/
void get_io_type(int *output_index, int *input_index) {
    int input_redir_pos = -1;
    int output_redir_pos = -1;

    int loop_counter = 0;
    while (arguments[loop_counter] != '\0') {
        if (strcmp(arguments[loop_counter], ">")) {

            if (output_redir_pos != -1) {
                // More than one output redirect is not supported
                *output_index = INVALID_REDIRECT;
                *input_index = INVALID_REDIRECT;
                break;
            }

            output_redir_pos = loop_counter;
        }

        if (strcmp(arguments[loop_counter], "<")) {

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
    if (input_redir_pos == -1 && output_redir_pos == -1) {
        *input_index = NO_REDIRECT;
        *output_index = NO_REDIRECT;
    }
    else {
        *input_index= input_redir_pos;
        *output_index = output_redir_pos;
    }
}

int arg_idx = 0;

// Built in command variables
char * built_in_cmd [] = { "cd", "jobs", "cat"};
int built_in_cmd_len = sizeof(built_in_cmd)/sizeof(built_in_cmd[0]);


// Returnerer current working directory
char * get_current_directory() {
    getcwd(cwd, sizeof(cwd));
    return cwd;
}

void emtpy_args () {
    memset(arguments, 0, BUFFER_LENGTH * sizeof(arguments[0]));
}

// Håndterer brukerinput og legger kommandoen (første string) til i cmnd, og argumentene i arguments
void handle_user_input(char input[BUFFER_LENGTH]) {
    // Flush buffers
    //memset(arguments, '\0', BUFFER_LENGTH * sizeof(char*));
    //memset(program, '\0', BUFFER_LENGTH * sizeof(char*));

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

    else if (p == 0) {
        // Retrieve the indices for the input and output redirect tokens, if they exist
        int input_redir_pos;
        int output_redir_pos;
        get_io_type(&input_redir_pos, &output_redir_pos);
        

        /*
        Five scenarios:
        1. No redirect: [-1, -1]
        2. Only input redirect [IN_POS, -1]
        3. Only output redirect [-1, OUT_POS]
        4. Input and thrn output redirect [IN_POS, OUT_POS]
        5. Invalid redirections [-2, -2]
        */

       if (output_redir_pos == INVALID_REDIRECT) {
           fprintf(stderr, "Invalid redirect syntax. Please try again.");
           exit(EXIT_FAILURE);
       }

        // If output redirect
        if (output_redir_pos != NO_REDIRECT) {
            /*
            close(STDOUT_FILENO);
            open(arguments[io_type[OUT_REDIR_IDX] + 1], O_CREAT|O_WRONLY|O_TRUNC);
            */
            // Replace STDOUT with the file after the output redirect token
            freopen(arguments[output_redir_pos + 1], "w", stdout);
        }

        // If input redirect
        if (input_redir_pos != NO_REDIRECT) {
            /*
            close(STDIN_FILENO);
            open(arguments[io_type[IN_REDIR_IDX] + 1], O_RDONLY);
            */
            // Replace STDIN with the file after the input redirect token
            freopen(arguments[input_redir_pos + 1], "r", stdin);
        } 
        
        if (output_redir_pos != NO_REDIRECT && input_redir_pos != NO_REDIRECT) {
            int first_delimiter = (output_redir_pos > input_redir_pos) ? input_redir_pos : output_redir_pos;
            set_cmd_subarray(0, first_delimiter - 1, program);
            execvp(cmnd, program);
        }
        else {
            execvp(cmnd, arguments);
        }
        
        exit(EXIT_SUCCESS);
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