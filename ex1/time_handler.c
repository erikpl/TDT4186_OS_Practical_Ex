#include <time.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

time_t current_time_as_secs() {
    return time(NULL);
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

    return 0;
}

int main() {
    time_t seconds;

    // When passed the parameter NULL, time defaults to returning the current time
    seconds = time(NULL);

    // Time-and-date struct to hold the current time
    struct tm* current_time;

    // Initialize the time-and-date struct based on current seconds
    current_time = localtime(&seconds);

    sleep(10);

    time_t new_time = time_t_from_tm_struct
}

