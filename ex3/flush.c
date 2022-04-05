#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/types.h>

#define BUFFER_LENGTH 700 

pid_t p;                       // Brukes til fork senere
int status;                    // Brukes til fork senere

char input[BUFFER_LENGTH];     // Input fra brukeren
char *word;                    // De individuelle stringene bra bruker
char *cmnd;                    // Første stringen i input fra bruker, altså kommandoen
char *args[BUFFER_LENGTH];     // Alt som kommer etter første string i input fra bruker
char *cd = "cd";
int x = 0;
int keep_running = 1;

// Returns the current working directory
char * getCurrentDirectory() {
char buf[BUFFER_LENGTH] = "";
char *path;
path = getcwd(buf, FILENAME_MAX);
return path;
}

// Henter ut brukerinput og lagrer det i cmnd og args
void getUserInput(){

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
        word = strtok(NULL, split_on);
        args[x] = word;
        x++;
    }
    
    // Bare en test for å sjekke om args har alle strengene, siste element er x-2 
    printf("The last string of the input is: %s \n",args[x-2]);
}


int main(int argc, char const *argv[]) {

while(keep_running) {

    getUserInput();



/*     printf("Cmnd: %s \n", cmnd);
    printf("First arg and second arg: %s, %s \n", args[0], args[1]);


    // Checks if the command is cd and if so, changes current directory. Arguments from input will be used instead of ".." later.
    while (strcmp(cmnd, cd) == 0){
        if(args[1] != NULL){
            printf("Too many arguments. \n");
            break;
        }
    
        if(args[1] == NULL) {
            printf("%s \n", args[0]);
            chdir("..");
            break; 
        }    
    } */


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