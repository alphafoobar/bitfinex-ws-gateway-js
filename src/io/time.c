#include <time.h>
#include "time.h"

double
find_time() {
    struct timespec time;
    int time_resp = clock_gettime(CLOCK_REALTIME, &time);
    if (time_resp == 0) {
        return time.tv_sec + (time.tv_nsec * 0.000000001);
    }
    return 0.0;
}