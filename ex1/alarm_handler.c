#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include "time_handler.c"
#include "alarm_structure.c"

pid_t child_pid;
pid_t parent_pid;

/**
 * Sets the PID passed in by reference to the current child PID.
 * @param[in] pid
*/
void set_child_pid(pid_t *pid) {
    *pid = child_pid;
}

void set_alarm(Alarm alarm) {
    
    // Returns the PID of the currently running (parent) process
    parent_pid = getpid();

    child_pid = fork();

    if (child_pid == 0) {
        time_t current_time = current_time_as_secs();
        // Time to the alarm should ring = alarm_time - current_time
        int time_to_alarm = (int) difftime(alarm.num_seconds, current_time);

        // Wait until the alarm should ring
        sleep(time_to_alarm);

        //Ring the alarm
        printf("Ring!\n");

        // Exit the child process
        exit(child_pid);
    }
    else {
        /* child_pid = wait(NULL);  */
    }

    
  

   /*  return 0; */
}