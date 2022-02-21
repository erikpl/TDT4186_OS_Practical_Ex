#include <stdio.h>
#include <time.h>
#include <signal.h>

#include "alarm_structure.c"
#include "alarm_handler.c"
/* #include "time_handler.c" */

#define SCHEDULE 's'
#define LIST 'l'
#define CANCEL 'c'
#define EXIT 'x'

// Init variables
int max_alarms = 3;
int curr_alarm = 0;
char choice;

int main() {
    // time_t current_secs = current_time_as_secs();
    // struct tm *current_time = tm_struct_from_time_t(&current_secs);
    // printf("Welcome to the alarm clock! It is currently %04d-%02d-%02d %02d-%02d-%02d\n",
    // // Add 1900 since tm_year denotes years since 1900
    // current_time -> tm_year + 1900,
    // // Add 1 since tm_mon is 0-indexed
    // current_time -> tm_mon + 1,
    // current_time -> tm_mday,
    // current_time -> tm_hour,
    // current_time -> tm_min,
    // current_time -> tm_sec);


    // Init array of alarms 
    Alarm all_alarms[max_alarms];

    //TODO Welcome the user with the current time

    while (choice != EXIT) {
        // Let the user choose what action to complete
        printf("\nPlease enter 's' (schedule), 'l' (list), 'c' (cancel), 'x' (exit): \n");
        scanf(" %c", &choice);

        waitpid(-1, 0, WNOHANG);

        // If the user chooses to schedule an alarm
        if (choice == SCHEDULE) {  

            // Check if we have room for more alarms
            if (max_alarms == curr_alarm) {
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
            all_alarms[curr_alarm] = CreateNewAlarm(child_pid, &uyear, &umonth, &uday, &uhour, &uminute, &usecond);

            //Printer child_pid, bare for å sjekke
            printf("The pid is: %d ", child_pid);

            // Print information about the new alarm
            printf("\nAlarm scheduled at %s", ctime(&all_alarms[curr_alarm].num_seconds));

            //Set the alarm for the right time
            set_alarm(&all_alarms[curr_alarm]);

            printf("%d ", all_alarms[curr_alarm].pid);

          
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
            if(cancel_pos < 0 || cancel_pos > curr_alarm)
            {
                printf("Invalid position! Please enter position between 1 to %d\n", curr_alarm);
            }
            else
            {
                printf("\n %d \n", all_alarms[cancel_pos-1].pid);
                fflush(stdout);
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