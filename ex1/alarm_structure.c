#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

// Initialize variables
int uyear, umonth, uday, uhour, uminute, usecond;

// Define the data structure for an alarm
typedef struct Alarm {
   struct tm tm_struct;
   time_t num_seconds;
   int pid;
} Alarm;


// To read how to format a tmStruct, read documentation: https://www.tutorialspoint.com/c_standard_library/time_h.htm
// Create a tm_struct of the user-input
struct Alarm CreateNewAlarm(int *uyear, int *umonth, int *uday, int *uhour, int *uminute, int *usecond) {

   // Init alarm
   Alarm alarm; 

   // Create time-struct using user-input
   alarm.tm_struct.tm_sec = *usecond;
   alarm.tm_struct.tm_min = *uminute;
   alarm.tm_struct.tm_hour = *uhour;
   alarm.tm_struct.tm_mday = *uday;
   alarm.tm_struct.tm_mon = *umonth - 1;
   alarm.tm_struct.tm_year = *uyear - 1900;

   // Create time_t (long int) and time_repr (string) from the tm_struct
   time_t num_seconds;
   num_seconds = mktime(&alarm.tm_struct);
   alarm.num_seconds = num_seconds;

  return alarm;
};
