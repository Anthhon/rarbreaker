#include <time.h>

long long current_time_ns(void)
{
#ifdef _WIN32
    return GetTickCount();
#else
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now.tv_sec * 1000000000LL + now.tv_nsec;
#endif
}

