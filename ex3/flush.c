#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/types.h>

#define BUFFER_LENGTH 700 

// Returns the current working directory
char * getCurrentDirectory() {
char buf[BUFFER_LENGTH] = "";
char *path;
path = getcwd(buf, FILENAME_MAX);
return path;
}

int main(int argc, char const *argv[])
{
    char input[BUFFER_LENGTH]; 
    char *word;
    char *cmnd;
    pid_t p;
    int status;
    char *cd = "cd";

    // Prompts the user with the current working directory plus ": "
    printf("%s: \n", getCurrentDirectory());

    // Gets the user input and stores it in input
    fgets(input, 200, stdin);

    // Splitting on space and tab
    char split_on[3] = {' ', '\t'};

    // Gets the first word from input 
    word = strtok(input, split_on);

    // Stores the first word as cmnd
    cmnd = word;

    // Keep getting words from input
    while (word != NULL) {
   
        //TODO: Få lagt til resten av ordene fra input i en liste eller lignende somehow
        word = strtok(NULL, split_on);
        printf("%s \n", word);
    
    }

    // Checks if the command is cd and if so, changes current directory. Arguments from input will be used instead of ".." later.
    if (strcmp(cmnd, cd) == 0){
        chdir("..");
        printf("%s: \n", getCurrentDirectory());
    }

    



 // Ignorer alt under her for now
 // Fork skal implementeres når ordene fra input legges i liste, denne listen skal brukes som argumenter i execl


/* 
    p = fork();

    if (p==-1){
        printf("There was an error while calling.");
    }

    if (p == 0) {
        execl("./run", "./run.c", NULL);
    }

    else {
        waitpid(p, &status, 0);
        int exit_status = WEXITSTATUS(status);
        printf("Now parent process is running! ");
        printf("The exit status of the child is: %d\n", exit_status);
        printf("The pid from here is: %d\n",  getpid());
    } */

}