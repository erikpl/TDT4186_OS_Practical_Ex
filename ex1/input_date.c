#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include<sys/wait.h>


// Create a tm_struct of the user-input
struct tm CreateTmStruct() {

  // Initialize variables
  int uyear, umonth, uday, uhour, uminute, usecond;

  // Prompt user
  printf("\nEnter: Year, Month, Day, Hour, Minute, Second: \n");
  
  // Assign user-input to values
  scanf("%d %d %d %d %d %d", &uyear, &umonth, &uday, &uhour, &uminute, &usecond);

  // Initialize struct
  struct tm tmStruct;

  // To read how to format a tmStruct, read documentation: https://www.tutorialspoint.com/c_standard_library/time_h.htm
  tmStruct.tm_sec = usecond;
  tmStruct.tm_min = uminute;
  tmStruct.tm_hour = uhour + 1;
  tmStruct.tm_mday = uday;
  tmStruct.tm_mon = umonth - 1;
  tmStruct.tm_year = uyear - 1900;

  return tmStruct;
};

time_t initialize(struct tm tm_struct){

   // Initialize time_t representation
      time_t time_repr;
      time_repr = mktime(&tm_struct);
    
   // Create a timestring
      char *timeString;
      timeString = ctime(&time_repr);

   // Print timestring
      printf("New alarm set for: \n");
      printf("%s", timeString );

      return time_repr;
}

void setAlarm(time_t time_repr) {
   //Creates a fork (Child id = 0)      
    int pid, parentID;
    pid = fork();

   //The child process
    if (pid == 0) {

   //Calculates seconds until alarm goes off
      time_t t;
      time(&t);
      double diff_t;
      diff_t = difftime(time_repr, t);

   //Waits the amount of seconds and then prints the alarm
      sleep(diff_t);
      printf("\n ALARM! \n");
        
   //Terminates child process
      exit(pid);           
}

   //The parent process
    else { 

   //Initialize and create a tm_struct
      struct tm tm_struct;
      tm_struct = CreateTmStruct();

   //Initialize time_t representation
      time_t time_repr;

   //Call function initialize with tm_struct as argument
      time_repr = initialize(tm_struct);
    
   // SetAlarm calls itself with time_repr as argument
      setAlarm(time_repr);
}
}

int main() {

   //Initialize and create a tm_struct
      struct tm tm_struct;
      tm_struct = CreateTmStruct();

   //Initialize time_t representation
      time_t time_repr;

   //Call function initialize with tm_struct as argument
      time_repr = initialize(tm_struct);

   // Call function SetAlarm with time_repr as argument
      setAlarm(time_repr);
      
    } 


   



