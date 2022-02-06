#include <stdio.h>
#include <time.h>

// Create a tm_struct of the user-input
struct tm CreateTmStruct() {

  // Initialize variables
  int uyear, umonth, uday, uhour, uminute, usecond;

  // Prompt user
  printf("Enter: Year, Month, Day, Hour, Minute, Second:");
  
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

int main() {

    // Initialize and create a tm_struct
    struct tm tm_struct;
    tm_struct = CreateTmStruct();

    // Initialize time_t representation
    time_t time_repr;
    time_repr = mktime(&tm_struct);
    
    // Create a timestring
    char *timeString;
    timeString = ctime(&time_repr);

    // Print timestring
    printf("%s", timeString);

    return 0;
}