#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

// Defining choice constants to make the code more readable
#define SCHEDULE 's'
#define LIST 'l'
#define CANCEL 'c'
#define EXIT 'x'

// Init constants and variables
int MAX_ALARMS = 3;
int curr_alarm = 0;
char choice;

pid_t child_pid, parent_pid;

// Initialize time variables
int uyear, umonth, uday, uhour, uminute, usecond;

// Define the data structure for an alarm
typedef struct Alarm {
   struct tm tm_struct;
   time_t num_seconds;
   pid_t pid;
} Alarm;


// To read how to format a tmStruct, read documentation: https://www.tutorialspoint.com/c_standard_library/time_h.htm
// Create a tm_struct of the user-input
struct Alarm create_new_alarm(pid_t pid, int *uyear, int *umonth, int *uday, int *uhour, int *uminute, int *usecond) {

   // Init alarm
   Alarm alarm; 

   // Create time-struct using user-input
   alarm.tm_struct.tm_sec = *usecond;
   alarm.tm_struct.tm_min = *uminute;
   alarm.tm_struct.tm_hour = *uhour;
   alarm.tm_struct.tm_mday = *uday;
   alarm.tm_struct.tm_mon = *umonth - 1;
   alarm.tm_struct.tm_year = *uyear - 1900;
   alarm.pid = pid;

   // Create time_t (long int) and time_repr (string) from the tm_struct
   time_t num_seconds;
   num_seconds = mktime(&alarm.tm_struct);
   alarm.num_seconds = num_seconds;

  return alarm;
};

void set_alarm(Alarm * alarm) {
    // getpid() retrieves the PID of the soon-to-be parent process
    parent_pid = getpid();
    // 0 if run from the child process
    // child_pid if run from the parent process
    child_pid = fork();

    // If run from the child process
    if (child_pid == 0) {
        // Current time as a time_t
        time_t current_time = time(NULL);

        // Time to the alarm should ring = alarm_time - current_time
        int time_to_alarm = (int) difftime(alarm->num_seconds, current_time);

        // Wait until the alarm should ring
        sleep(time_to_alarm);

        //Ring the alarm
        #ifdef __linux__
        // Does not work on WSL
        execlp("mpg123", "mpg123", "alarm_fx.mp3", NULL);
        // If ran on a Mac
        #elif __APPLE__
        execlp("afplay", "afplay", "alarm_fx.mp3", NULL);
        #else
        printf("Ring!\n");
        #endif

        // Exit the child process
        exit(child_pid);
    }
    else {
        alarm->pid=child_pid;
    }
}

int main() {
    // Current time in seconds
    time_t current_secs = time(NULL);
    // Current time as a struct
    struct tm *current_time = localtime(&current_secs);
    printf("Welcome to the alarm clock! It is currently %04d-%02d-%02d %02d-%02d-%02d\n",
    // // Add 1900 since tm_year denotes years since 1900
    current_time -> tm_year + 1900,
    // // Add 1 since tm_mon is 0-indexed
    current_time -> tm_mon + 1,
    current_time -> tm_mday,
    current_time -> tm_hour,
    current_time -> tm_min,
    current_time -> tm_sec);

    // Init array of alarms 
    Alarm all_alarms[MAX_ALARMS];

    while (choice != EXIT) {
        // Let the user choose what action to complete
        printf("\nPlease enter 's' (schedule), 'l' (list), 'c' (cancel), 'x' (exit): \n");
        scanf(" %c", &choice);

        // Killing zombies!
        waitpid(-1, 0, WNOHANG);

        // If the user chooses to schedule an alarm
        if (choice == SCHEDULE) {  

            // Check if we have room for more alarms
            if (MAX_ALARMS == curr_alarm) {
                printf("\nCurrently at maximum amount of alarms\n");
                continue;
            }


            int child_pid = 0;
            // Initialize time-variables
            int uyear, umonth, uday, uhour, uminute, usecond;

            // Prompt user and assign user-input to prompt-values
            printf("\nSchedule alarm at which date and time (YYYY/MM/DD-HH:MM:SS)? \n");
            scanf("%d/%d/%d-%02d:%02d:%02d", &uyear, &umonth, &uday, &uhour, &uminute, &usecond);

            // Create new alarm and add it to list of alarms. Then, iterate counter
            all_alarms[curr_alarm] = create_new_alarm(child_pid, &uyear, &umonth, &uday, &uhour, &uminute, &usecond);

            // Print information about the new alarm
            printf("\nAlarm scheduled at %s", ctime(&all_alarms[curr_alarm].num_seconds));

            //Set the alarm for the right time
            set_alarm(&all_alarms[curr_alarm]);
          
            // Iterate counter, letting us populate next slot in all_alarms array
            curr_alarm ++;
        }
        
        // If the user chooses to list all alarms
        if (choice == LIST) {
            
            // Purely for aesthetics
            printf("\n");
            
            // Print all the time representations of the alarms            
            for (int i = 0; i < curr_alarm; ++i) {

                // NB NB!!!! ctime calls use statically allocated buffers for the return string. Therefore, we call ctime on num_seconds instead of storing the char array as this lets us avoid referencing issues. See more at https://www.ibm.com/docs/en/i/7.3?topic=functions-ctime-convert-time-character-string
                printf("Alarm %d at %s", i, ctime(&all_alarms[i].num_seconds));
            }
        }

        // If the user chooses to cancel a given alarm
        if (choice == CANCEL) {
            // Prompt user and assign user-input to prompt-values
            int cancel_pos;
            printf("\nCancel which alarm? \n");
            scanf("%d", &cancel_pos);

            // Make sure the user inputs a valid position
            if (cancel_pos < 0 || cancel_pos > curr_alarm) {
                printf("Invalid position! Please enter position between 1 to %d\n", curr_alarm);
            }
            else {
                printf("\n %d \n", all_alarms[cancel_pos-1].pid);
                // Flush the standard output
                fflush(stdout);
                // Terminate the alarm
                kill(all_alarms[cancel_pos-1].pid, SIGKILL);

                // Deletes the alarm at cancel_pos by copying over all alarms to the right
                // This ensures adding and deleting a lot of alarms will not force us to restart the program
                // and makes the alarm numbers update correctly
                for (int i = cancel_pos; i < curr_alarm - 1; i++)
                {
                    all_alarms[i] = all_alarms[i + 1];
                }

                printf("\nAlarm number %d cancelled.\n", cancel_pos);                
                // Decrement the current alarm counter by one to correctly assign next alarm
                curr_alarm--;
                }
            }
    }

    return 0;
}