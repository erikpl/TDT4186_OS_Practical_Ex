#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#include "alarm_structure.c"
#include "time_handler.c"

void set_alarm(Alarm *alarm) {
    pid_t child_pid, parent_pid;
    parent_pid = getpid();

    child_pid = fork();

    if (getpid() == child_pid) {
        time_t current_time = current_time_as_secs();
        // Time to the alarm should ring = alarm_time - current_time
        int time_to_alarm = (int) difftime(alarm->num_seconds, current_time);
        // Wait until the alarm should ring
        sleep(time_to_alarm);
        // Exit the child process
        exit(child_pid);
    }
    else {
        // 
    }

    return 0;
}