#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/wait.h>

#define BUFFER_LENGTH 700

// Globale variabler
pid_t p;                      
int status;  
int x = 0;                  
char *cmnd; 
char cwd[BUFFER_LENGTH];
char *arguments[BUFFER_LENGTH];

// Returnerer current working directory
char * get_current_directory(){
    getcwd(cwd, sizeof(cwd));
    return cwd;
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
        word = strtok(NULL, split_on);
        arguments[x] = word;
        x++;
    }

    
    // Sjekker om kommandoen er cd
    if(strcmp(cmnd, "cd") == 0) {
        printf("Arguments = %s\n", arguments[0]);
        if(arguments[0] == NULL) {
            printf("The path is empty.\n");
        }

        chdir(arguments[0]);
    }  
}

void execute_commands() {
    pid_t p = fork();

    if (p == -1) {
        printf("There was an error when trying to fork.\n");
        return;

    }

    else if (p > 0) {
        if(waitpid(p, &status, 0) == -1) {
            printf("Error: Failed calling waitpid.\n");
            exit(EXIT_FAILURE);
        }
        
        if(WIFEXITED(status)) {
            int exit_status;
            exit_status = WEXITSTATUS(status);
     
            printf("Exit status [%s %s] = %d\n", cmnd, arguments[0], exit_status);
        }
    }

    else if (p == 0) {

        execvp(cmnd, arguments);
        printf("There was an error: %s\n", strerror(errno));
        exit(0);
    }

    else {
        wait(NULL);
        return;
    }
}

int main() {
    int keep_running = 1;
    char user_input[BUFFER_LENGTH];

    while (keep_running) {
        printf("%s: ", get_current_directory());
        fgets(user_input, BUFFER_LENGTH, stdin);
        fflush(stdin);
        printf("\n");

        // Fjerner mellomrommet på slutten av user_input
        user_input[strlen(user_input) - 1] = '\0';

        handle_user_input(user_input);

        if (strcmp(cmnd, "cd") != 0) {
            execute_commands();
        }
    }
}