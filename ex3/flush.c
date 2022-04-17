#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>

#define BUFFER_LENGTH 700

// General global variables
int status;  
char *cmnd;
char cwd[BUFFER_LENGTH];

// Arguments variables
char *arguments[BUFFER_LENGTH];
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

    // Child_pid is 0 when we are in the child process. Run command
    else if (child_pid == 0) {

        // Execute command
        execvp(cmnd, arguments);

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