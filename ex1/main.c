#include <stdio.h>
#include <time.h>
#include <signal.h>

#include "alarm_structure.c"
#include "alarm_handler.c"
#include "time_handler.c"

#define SCHEDULE 's'
#define LIST 'l'
#define CANCEL 'c'
#define EXIT 'x'
#define MAX_ALARMS 3

// Init variables
int current_alarm = 0;
char choice;

int main() {
    time_t current_secs;
    current_secs = time(NULL);
    struct tm *current_time = localtime(current_time);
    printf("Welcome to the alarm clock! It is currently %04d-%02d-%02d %02d:%02d:%02d\n",
    // Add 1900 since tm_year denotes years since 1900
    current_time -> tm_year + 1900,
    // Add 1 since tm_mon is 0-indexed
    current_time -> tm_mon + 1,
    current_time -> tm_mday,
    current_time -> tm_hour,
    current_time -> tm_min,
    current_time -> tm_sec);

    // Init array of alarms 
    Alarm all_alarms[MAX_ALARMS];

    int wait_status;
    pid_t pid;

    /** 
     * Catch and terminate zombie processes.
     * waitpid will wait until the child processes are done and claim their exit status.
     * The WNOHANG option specifies that this waiting will be non-blocking.
     * -1 means any child process.
     * >0 means wait for the process with the same PID as pid 
    */
    // while (pid = waitpid(-1, &wait_status, WNOHANG) > 0);

    while (choice != EXIT) {
        // Let the user choose what action to complete
        printf("\nPlease enter 's' (schedule), 'l' (list), 'c' (cancel), 'x' (exit): \n");
        scanf(" %c", &choice);

        // If the user chooses to schedule an alarm
            if (choice == SCHEDULE) {
                // Check if we have room for more alarms
                if (current_alarm == MAX_ALARMS) {
                    printf("\nCurrently at maximum amount of alarms\n");
                    continue;
                }

                // Initialize time-variables
                int uyear, umonth, uday, uhour, uminute, usecond;

                // Prompt user and assign user-input to prompt-values
                printf("\nSchedule alarm at which date and time (YYYY/MM/DD-HH:MM:SS)? \n");
                // TODO: validering av input
                scanf("%d/%d/%d-%02d:%02d:%02d", &uyear, &umonth, &uday, &uhour, &uminute, &usecond);

                //Set the alarm for the right time
                set_alarm(&all_alarms[current_alarm]);

                Alarm alarm;
                alarm.tm_struct.tm_sec = usecond;
                alarm.tm_struct.tm_min = uminute;
                alarm.tm_struct.tm_hour = uhour + 1;
                alarm.tm_struct.tm_mday = uday;
                // Months are zero-indexed in the tm struct
                alarm.tm_struct.tm_mon = umonth - 1;
                // Years are 1900-indexed in the tm struct
                alarm.tm_struct.tm_year = uyear - 1900;

                time_t num_seconds;
                num_seconds = mktime(&(alarm.tm_struct));
                alarm.num_seconds = num_seconds;

                time_t current_time;

                current_time = time(NULL);

                pid_t return_code;

                // Mulig denne må fikses
                waitpid(-1, &wait_status, WNOHANG);

                return_code = fork();

                if (return_code == 0) {
                    time_t sleep_time = alarm.num_seconds - current_time;
                    // Print information about the new alarm
                    printf("\nAlarm scheduled at %s", ctime(&all_alarms[current_alarm].num_seconds));
                    // Wait for the alarm to complete
                    sleep(sleep_time);
                    
                    // Alarm notification mechanism depends on the OS
                    #ifdef _WIN32
                        printf("RING!\n");
                    #elif __APPLE__
                        execlp("afplay", "afplay", "./alarm_fx.mp3", 0);
                    #elif __unix__
                        execlp("mpg123", "mpg123", "-q", "./alarm_fx.mp3", 0);
                    #endif

                    exit(0);
                }

                else {
                    alarm.pid = return_code;
                    all_alarms[current_alarm] = alarm;
                    // Iterate counter, letting us populate next slot in all_alarms array
                    current_alarm++;   
                }
            }
        
        // If the user chooses to list all alarms
        else if (choice == LIST) {
            
            // Purely for aesthetics
            printf("\n");
            
            // Print all the time representations of the alarms            
            for (int i = 0; i < current_alarm; i++) {
                // NB NB!!!! ctime calls use statically allocated buffers for the return string. Therefore, we call ctime 
                // on num_seconds instead of storing the char array as this lets us avoid referencing issues. 
                // See more at https://www.ibm.com/docs/en/i/7.3?topic=functions-ctime-convert-time-character-string
                printf("Alarm %d at %s", i, ctime(&all_alarms[i].num_seconds));
            }
        }

        // If the user chooses to cancel a given alarm
        else if (choice == CANCEL) {
            // Prompt user and assign user-input to prompt-values
            int cancel_pos;
            printf("\nCancel which alarm? \n");
            scanf("%d", &cancel_pos);

            // Make sure the user inputs a valid position
            if (cancel_pos < 0 || cancel_pos > current_alarm) {
                printf("Invalid position! Please enter position between 1 to %d\n", current_alarm);
            }
            else {
                // Deletes the alarm at cancel_pos by copying over all alarms to the right
                // This ensures adding and deleting a lot of alarms will not force us to restart the program
                // and makes the alarm numbers update correctly
                for (int i = cancel_pos; i < current_alarm - 1; i++) {
                    all_alarms[i] = all_alarms[i + 1];
                }

                kill(all_alarms[cancel_pos-1].pid, SIGKILL);
            
                printf("\nAlarm number %d cancelled.\n", cancel_pos);                
                // Decrement the current alarm counter by one to correctly assign next alarm
                current_alarm--;
            }
        }
        else {
            printf("ERROR: Invalid input. Please try again.");
        }
    }
}