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
struct Alarms CreateAlarmStruct(int* uyear, int* umonth, int* uday, int* uhour, int* uminute, int *usecond) {

   // Init alarm
   Alarm alarm;

   // Create time-struct using user-input
   alarm.tm_struct.tm_sec = *usecond;
   alarm.tm_struct.tm_min = *uminute;
   alarm.tm_struct.tm_hour = *uhour;
   alarm.tm_struct.tm_mday = *uday;
   alarm.tm_struct.tm_mon = *umonth - 1;
   alarm.tm_struct.tm_year = *uyear - 1900;

   // Create a timestring
   // time_t time_repr;
   // time_repr = mktime(&alarm.tm_struct);
   // char char_time = ctime(&time_repr);
   // alarm.time_repr = &char_time;

  return alarm;
};

// time_t initialize(struct tm tm_struct){

//       // Initialize time_t representation
//       time_t time_repr;
//       time_repr = mktime(&tm_struct);
    
//       // Create a timestring
//       char *timeString;
//       timeString = ctime(&time_repr);

//       // Print timestring
//       printf("New alarm set for: \n");
//       printf("%s", timeString );

//       return time_repr;
// }

void setAlarm(int* uyear, int* umonth, int* uday, int* uhour, int* uminute, int *usecond) {
   //Creates a fork (Child id = 0)     
   int child_pid, parent_pid;

   child_pid = fork();
   parent_pid = getpid();

   Alarm new_alarm = CreateAlarmStruct(uyear, umonth, uday, uhour, uminute, usecond);
   new_alarm.pid = child_pid;
   printf("%d child pid, %d parent PID\n\n\n", child_pid, parent_pid);
  // printf("%d timestring", new_alarm.time_repr);



//    //The child process
//     if (pid == 0) {

//       //Calculates seconds until alarm goes off
//       time_t t;
//       time(&t);
//       double diff_t;
//       diff_t = difftime(time_repr, t);

//       //Waits the amount of seconds and then prints the alarm
//       sleep(diff_t);
//       printf("\n ALARM! \n");
      
//       //Terminates child process
//       exit(pid);           
//    }

//    //The parent process
//     else { 

//       printf("%d", pid);

//       //Initialize and create a tm_struct
//       struct tm tm_struct;
      
//       tm_struct = CreateAlarmStruct(&pid, &uyear, &umonth, &uday, &uhour, &uminute, &usecond);

//       //Initialize time_t representation
//       time_t time_repr;

//       //Call function initialize with tm_struct as argument
//       time_repr = initialize(tm_struct);

//       // SetAlarm calls itself with time_repr as argument
//       setAlarm(time_repr);
// }
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