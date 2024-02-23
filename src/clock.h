#ifndef CLOCK_H
#define CLOCK_H

#define START_CLOCK() _start = clock()
#define STOP_CLOCK() _end = clock()
#define CALCULATE_TIME() _cpu_time_used = ((double) (_end - _start) / CLOCKS_PER_SEC)
#define RAND_IN_RANGE(x) (rand() % (x))

long long current_time_ns(void);

#endif // CLOCK_H
