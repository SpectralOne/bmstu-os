#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

int main(void) {
    time_t raw_time;
    struct tm *time_info;
    char buf[70];

    while(1)
    {
        time(&raw_time);
        time_info = localtime(&raw_time);
        puts((asctime(time_info)));
        sleep(10);
    }

    return 0;
}
