#ifndef TIME_HANDLER
#define TIME_HANDLER
#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

/**
 * Sets the referenced time_t object to the current number of seconds
 * 
 * @param current_time 
 */
void set_current_time(time_t *current_time) {
    *current_time = time(NULL);
}

time_t time_t_from_tm_struct(struct tm *tm_struct) {
    return mktime(tm_struct);
}

struct tm *tm_struct_from_time_t(time_t *time_as_secs) {
    return localtime(time_as_secs);
}

void set_tm_struct_with_time_values(int *uyear, int *umonth, int *uday, int *uhour, int *uminute, int *usecond, struct tm *tm_struct) {
    tm_struct -> tm_year = *uyear;
    tm_struct -> tm_mon = *umonth;
    tm_struct -> tm_mday = *uday;
    tm_struct -> tm_hour = *uhour;
    tm_struct -> tm_min = *uminute;
    tm_struct -> tm_sec = *usecond;

    /* return 0; */
}
#endif // TIME_HANDLER