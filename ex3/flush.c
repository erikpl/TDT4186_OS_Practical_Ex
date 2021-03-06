#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>

#define BUFFER_LENGTH 700
#define LEN(arr) ((int) (sizeof (arr) / sizeof (arr)[0]))
#define INVALID_REDIRECT -2
#define NO_REDIRECT -1


// General global variables
int status;
char *cmnd;
char cwd[BUFFER_LENGTH];
char user_input[BUFFER_LENGTH];

// Arguments variables
char * arguments[BUFFER_LENGTH];
int arg_idx = 0;

// Built in command variables
char * built_in_cmd [] = { "cd", "jobs"};
int built_in_cmd_len = sizeof(built_in_cmd) / sizeof(built_in_cmd[0]);

// Background processes, maximum 100
int bg_child_pids[100];
int bg_child_args[100];
char bg_child_args_history[1000][BUFFER_LENGTH][20];
int bg_idx = 0;
int bg_abs_idx = 0;
int bg_proc = 0;


// Returnerer current working directory
char * get_current_directory() {
    getcwd(cwd, sizeof(cwd));
    return cwd;
}

// Nulls out remaining arguments, including the first redirection delimiter.
void set_subarray_from_args(int start_index) {
    int counter = start_index;

    /* There is a confusion between NULL-ing out the argument array and
    *  and zeroing it out when flushing. This is because execvp expects a 
    *  NULL-terminated array. Since zeroing out is always done and used before 
    *  NULL-ing out, this works.
    */
    while (arguments[counter] != NULL) {
        arguments[counter] = NULL;
        counter++;
    }
}

void emtpy_args () {
    memset(arguments, 0, BUFFER_LENGTH * sizeof(arguments[0]));
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

        // If the argument is "&", run in background        
        if (!strcmp(word, "&")) {
            bg_proc = 1;
        }

        // Add argument to list
        else {
            arguments[arg_idx] = word;
        }
        word = strtok(NULL, split_on);
        arg_idx++;
    }
    // Reset counter
    arg_idx = 0;
}


void execute_built_in() {

    // If the command is cd, change directory with built in chdir function
    if(strcmp(cmnd, "cd") == 0) {
        if(arguments[0] == NULL) {
            printf("The path is empty.\n");
        }
        chdir(arguments[1]);
    }
    
    // Check running background jobs
    if (strcmp(cmnd, "jobs") == 0) {

        // Loop through all child pids    
        for(int loop_idx = 0; loop_idx < bg_idx; loop_idx++) {
            
            // Print process ID 
            printf("PID %d: %d. ", loop_idx, bg_child_pids[loop_idx]);

            printf("Command line [");
            int bg_arg_idx = 0;

            // While the arguments are not "&". Find the correct index in the arguments history with the bg_child_args list. 
            while (strcmp(bg_child_args_history[bg_child_args[loop_idx]][bg_arg_idx], "\0" ) != 0  ) {
                printf(" %s", bg_child_args_history[bg_child_args[loop_idx]][bg_arg_idx]);
                bg_arg_idx++;
            }

            // Printing the "&" we removed
            printf(" &");
            bg_arg_idx = 0;
            printf(" ] = 0\n");

            // }
        }
        if (bg_idx == 0) {
            puts("No background jobs currently running.");
        }
    }
}

int get_first_delimiter(int output_redir_pos, int input_redir_pos) {
    if (output_redir_pos == -1) {
        return input_redir_pos;
    }
    else if (input_redir_pos == -1) {
        return output_redir_pos;
    }
    else {
        return (input_redir_pos > output_redir_pos) ? output_redir_pos : input_redir_pos;
    }
}


void get_io_type(int *output_index, int *input_index) {
    int input_redir_pos = -1;
    int output_redir_pos = -1;

    int loop_counter = 0;
    
    while (arguments[loop_counter] != 0) {
        if (strcmp(arguments[loop_counter], ">") == 0) {
            if (output_redir_pos != -1) {
                // More than one output redirect is not supported
                *output_index = INVALID_REDIRECT;
                *input_index = INVALID_REDIRECT;
                break;
            }

            output_redir_pos = loop_counter;
        }

        if (strcmp(arguments[loop_counter], "<") == 0) {
            if (input_redir_pos != -1) {
                // More than one input redirect is not supported
                *output_index = INVALID_REDIRECT;
                *input_index = INVALID_REDIRECT;
                break;
            }
            input_redir_pos = loop_counter;
        }

        if (output_redir_pos > input_redir_pos) {
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
        *input_index = input_redir_pos;
        *output_index = output_redir_pos;
    }
}

void execute_bin();

void cmnd_prompt() {
        fflush(stdin);
        fflush(stdout);

        printf("\n%s: ", get_current_directory());

        fgets(user_input, BUFFER_LENGTH, stdin);
        fflush(stdin);
        printf("\n");
}

int main() {
    int is_executed = 0;
    char * line[BUFFER_LENGTH];

    while (1) {

        // Get user input
        cmnd_prompt();

        // Fjerner mellomrommet på slutten av user_input
        user_input[strlen(user_input) - 1] = '\0';

        handle_user_input(user_input);


        // If user presses ctrl-d and thus no inputs are given.
        if (arguments[0] == NULL ) {
            printf("Exiting flush.");
            return 0;
        }

        // Check if the command is built into the shell
        for (int i = 0; i < built_in_cmd_len; ++i) {

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


void execute_bin() {
    // Get PIDs
    pid_t parent_pid = getpid();
    pid_t child_pid = fork();
    pid_t curr_pid = getpid();

    if (curr_pid == -1 && child_pid == 0) {
        fprintf(stderr, "\nThere was an error when trying to fork.\n");
        return;
    }

    // For the parent process, print the status of the child process execution
    if (child_pid > 0) {

        // Put a process in the background
        if (bg_proc) {

            // Store the child PID
            bg_child_pids[bg_idx] = child_pid;

            // Store the arguments in the child array.
            int child_arg_idx = 0;

            // Get the command arguments
            while (arguments[child_arg_idx]) { 
                strcpy(bg_child_args_history[bg_abs_idx][child_arg_idx], arguments[child_arg_idx]);
                bg_child_args[bg_idx] = bg_abs_idx;
                child_arg_idx++;
            }

            // Reset the number of arguments counter
            child_arg_idx = 0;

            // Next background process
            bg_abs_idx++;
            bg_idx++;
        }

        // Check background processes
        for(int i = 0; i < LEN(bg_child_pids); ++i) {
            // Check if each process is finished
            int finished = waitpid(bg_child_pids[i], &status, WNOHANG);

            // Unused bg slots will also eval to -1
            if (finished == -1 && bg_child_pids[i] != 0 ) {

                // Print exit status of process
                printf("\nExit status [");
                int bg_arg_idx = 0;

                // While the arguments are not "&". Find the correct index in the arguments history with the bg_child_args list. 
                while (strcmp(bg_child_args_history[bg_child_args[bg_idx]][bg_arg_idx], "\0" ) != 0  ) {
                    printf(" %s", bg_child_args_history[bg_child_args[bg_idx]][bg_arg_idx]);
                    bg_arg_idx++;
                }

                // Printing the "&" we removed
                printf(" &");
                bg_arg_idx = 0;
                printf(" ] = 0\n");

                // Delete the finished process.
                for (int cancel_idx = i; cancel_idx < 100 - 1; cancel_idx++) {
                        bg_child_pids[cancel_idx] = bg_child_pids[cancel_idx + 1];
                        bg_child_args[cancel_idx] = bg_child_args[cancel_idx + 1];
                }

                // }
                // Decrement the current background counter by one to correctly assign next background process
                bg_idx--;
            }
        }

        // If we run processes normally, not in background
        if (bg_proc != 1) {

            // Wait for process to finish
            if(waitpid(child_pid, &status, 0) == -1) {

                fprintf(stderr, "Error: Failed calling waitpid.\n");
                exit(EXIT_FAILURE);
            }

            if(WIFEXITED(status)) {

                int exit_status = WEXITSTATUS(status);

                // Print the status of the exec
                printf("\nExit status [");
                int arg_prnt_idx = 0;

                // While there still are arguments
                while (arguments[arg_prnt_idx]) {
                    printf(" %s", arguments[arg_prnt_idx]);
                    arg_prnt_idx++;
                }
                arg_prnt_idx = 0;
                printf(" ] = %d", exit_status);
            }
        }
        bg_proc = 0;
        fflush(stdout);
    }

    // Child_pid is 0 when we are in the child process. Run command.
    else if (child_pid == 0) {

        // Execute command
        setpgid(0, 0);
        int input_redir_pos;
        int output_redir_pos;
        // Sets redirect indices. -1 for no redirect, -2 for invalid redirect.
        get_io_type(&output_redir_pos, &input_redir_pos);

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
            int first_delimiter = get_first_delimiter(output_redir_pos, input_redir_pos);

            printf("First delimiter: %d\n", first_delimiter);
            set_subarray_from_args(first_delimiter);
        }

        execvp(cmnd, arguments);

        // Return error from command-execution
        fprintf(stderr, "There was an error: %s\n", strerror(errno));
        exit(EXIT_SUCCESS);
    }
}
