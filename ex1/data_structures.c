#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <stdlib.h>

// Create a new datatype, Alarm
typedef struct Alarm {
    // time_t and string representation of current time
    time_t currentTime;
    char *currentTimeString;
    
    // time_t and string representation of alarm time
    time_t alarmTime;
    char *alarmTimeString;
    
    // boolean determining whether alarm is active or not
    bool isActive;
} alarm;

// Initialize the alarm by setting current-time, and a human readable timeString
void initAlarm(alarm *a) {
   time(&a->currentTime);
   a->currentTimeString = ctime(&a->currentTime);
};

// Schedule an alarm
void scheduleAlarm(int *alarmCount, int *maxAlarms, alarm *allAlarms) {

    //TODO run input_date.c here to set alarmTime and alarmTimeString
    initAlarm(&allAlarms[*alarmCount]);    
    allAlarms[*alarmCount].isActive = false;
};


int main() {

    // Create a counter for alarms
    int maxAlarms = 50;
    int alarmCount = 0;

    // Array that holds all created alarms
    alarm allAlarms[maxAlarms];

    // Create a new Alarm
    scheduleAlarm(&alarmCount, &maxAlarms, &allAlarms);

    printf("Is active? %i\n", allAlarms[0].isActive);
    printf("Current time = %d\n", allAlarms[0].currentTime);
    printf("Current time = %s\n", allAlarms[0].currentTimeString);

    return 0;
}