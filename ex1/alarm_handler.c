#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include "time_handler.c"
#include "alarm_structure.c"

#define IS_CHILD 0

pid_t child_pid;
pid_t parent_pid;

/**
 * Sets the PID passed in by reference to the current child PID.
 * @param[in] pid
*/
void set_child_pid(pid_t *pid) {
    *pid = child_pid;
}

void set_alarm(Alarm *alarm) {
    
    // Returns the PID of the currently running (parent) process
    parent_pid = getpid();

    // PID of the child process if returned to the parent process
    // 0 if returned to the child process
    // -1 if the fork failed
    child_pid = fork();
    
    // From here on out, the same code is run by both processes

    // If run from the child process
    if (child_pid == IS_CHILD) {
        time_t current_time; 
        set_current_time(&current_time);
        // Time to the alarm should ring = alarm_time - current_time
        int time_to_alarm = (int) difftime(alarm -> num_seconds, current_time);

        // Wait until the alarm should ring
        sleep(time_to_alarm);

        // Ring the alarm
        printf("Ring!\n");

        // Exit the child process
        // Return 0 for success
        exit(0);
    }
    else {
        // If the child process ran successfully, child_process_rc will be set to 0
        int child_process_rc = wait(NULL);
    }
}