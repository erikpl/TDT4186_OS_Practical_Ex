#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>

#define BUFFER_LENGTH 700
#define LEN(arr) ((int) (sizeof (arr) / sizeof (arr)[0]))

// General global variables
int status;
char *cmnd;
char cwd[BUFFER_LENGTH];

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
        for(int i = 0; i < LEN(bg_child_pids); ++i) {
            if (bg_child_pids[i] != 0) {
            
            // Print process ID 
            printf("PID %d: %d. ", i, bg_child_pids[i]);

            printf("Command line [");
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

            }
        }
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
                // Delete the finished process. Because we use an absolute history and keep the index for the correct process, we dont have to reassign our matrix and do expensive calculation. The cost is extra storage of all args history.
                for (int cancel_idx = i; cancel_idx < bg_idx - 1; cancel_idx++) {
                        bg_child_pids[i] = bg_child_pids[i + 1];
                        bg_child_args[i] = bg_child_args[i + 1];
                }
                // Decrement the current background counter by one to correctly assign next background process
                bg_idx--;
            }
        }

        // If we run processes normally, not in background
        if (bg_proc != 1) {

            // Wait for process to finish
            if(waitpid(child_pid, &status, 0) == -1) {

                printf("Error: Failed calling waitpid.\n");
                exit(EXIT_FAILURE);
            }

            if(WIFEXITED(status)) {

                int exit_status = WEXITSTATUS(status);

                // if (bg_proc != 1) {
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
        execvp(cmnd, arguments);

        // Return error from command-execution
        printf("There was an error: %s\n", strerror(errno));
        exit(0);
    }
}

int main() {

    char user_input[BUFFER_LENGTH];
    int is_executed = 0;
    char * line[BUFFER_LENGTH];

    while (1) {

        // Get user input

        printf("\n%s: ", get_current_directory());
        fflush(stdout);

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