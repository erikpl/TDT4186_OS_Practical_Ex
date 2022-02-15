#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

// Initialize variables
int uyear, umonth, uday, uhour, uminute, usecond;

typedef struct Alarms {
   struct tm tm_struct;
   int pid;
 //  char time_repr;
} Alarm;


// To read how to format a tmStruct, read documentation: https://www.tutorialspoint.com/c_standard_library/time_h.htm
// Create a tm_struct of the user-input
// TODO: Return alarm or alarm pointer?
struct Alarms CreateAlarmStruct(int* pid, int* uyear, int* umonth, int* uday, int* uhour, int* uminute, int *usecond) {

   // Init alarm
   Alarm alarm;

   // Create time-struct using user-input
   alarm.tm_struct.tm_sec = *usecond;
   alarm.tm_struct.tm_min = *uminute;
   alarm.tm_struct.tm_hour = *uhour;
   alarm.tm_struct.tm_mday = *uday;
   alarm.tm_struct.tm_mon = *umonth - 1;
   alarm.tm_struct.tm_year = *uyear - 1900;

   alarm.pid = *pid;

   // Create a timestring
   // time_t time_repr;
   // time_repr = mktime(&alarm.tm_struct);
   // char char_time = ctime(&time_repr);
   // alarm.time_repr = &char_time;

  return alarm;
};


void setAlarm(int* uyear, int* umonth, int* uday, int* uhour, int* uminute, int *usecond) {
   int child_pid, parent_pid;
   // Calling getpid before forking will return the pid of the parent
   parent_pid = getpid();
   // Gets child pid from fork
   child_pid = fork();
   // From here, both processes run the same code

   if (getpid() == child_pid) {
      time_t t;
      time(&t);
      double diff_t;
      diff_t = difftime(time_repr, t);

      //Waits the amount of seconds and then prints the alarm
      sleep(diff_t);
      printf("\n ALARM! \n");
      
      //Terminates child process
      exit(child_pid); 
   }
   else {
      //Initialize and create a tm_struct
      struct tm tm_struct;
      
      Alarm new_alarm = CreateAlarmStruct(&parent_pid, &uyear, &umonth, &uday, &uhour, &uminute, &usecond);

     //Initialize time_t representation
      time_t time_repr;

      //Call function initialize with tm_struct as argument
      time_repr = initialize(tm_struct);
      // SetAlarm calls itself with time_repr as argument
      setAlarm(time_repr);
   }


int main() {

      // Prompt user and assign user-input to prompt-values
      printf("\nEnter: Year, Month, Day, Hour, Minute, Second: \n");
      scanf("%d %d %d %d %d %d", &uyear, &umonth, &uday, &uhour, &uminute, &usecond);

      // Call function setAlarm with user-input
      setAlarm(&uyear, &umonth, &uday, &uhour, &uminute, &usecond);


   //    //Initialize and create a tm_struct
   //    struct tm tm_struct;
   //    int pid;
   //  //  tm_struct = CreateAlarmStruct(&pid, &uyear, &umonth, &uday, &uhour, &uminute, &usecond);

   //    //Initialize time_t representation
   //    time_t time_repr;

   //    //Call function initialize with tm_struct as argument
   //    time_repr = initialize(tm_struct);


      return 0;
    }