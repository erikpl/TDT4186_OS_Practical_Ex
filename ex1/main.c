#include <stdio.h>
#include <time.h>

#define SCHEDULE 's'
#define LIST 'l'
#define CANCEL 'c'
#define EXIT 'x'

int main() {
    // Declares a time struct to hold the current time as number of seconds
    time_t seconds;
    // When passed the parameter NULL, time defaults to returning the current time
    seconds = time(NULL);
    // Time-and-date struct to hold the current time
    struct tm* current_time;
    // Initialize the time-and-date struct based on current seconds
    current_time = localtime(&seconds);

    // Offset current year by 1900 to get number of years since year 0
    current_time -> tm_year = current_time -> tm_year + 1900;

    // Offset current month by 1 to account for 0-indexation
    current_time -> tm_mon = current_time -> tm_mon + 1;

    // User prompt with a welcome message and the current time
    printf("Welcome to the alarm clock! It is currently %04d-%02d-%02d %02d-%02d-%02d\n",
    current_time -> tm_year,
    current_time -> tm_mon,
    current_time -> tm_mday,
    current_time -> tm_hour,
    current_time -> tm_min,
    current_time -> tm_sec);

    // Prompt user to enter instructin
    printf("Please enter \"s\" (schedule), \"l\" (list), \"c\" (cancel), \"x\" (exit)\n");
    char instruction;
    scanf("%c", &instruction);
    printf("You chose to do a %c", instruction);

    return 0;
}