#define TIMEVAL_TO_TIMESPEC(tv, ts)                                     \
        do {                                                            \
                (ts)->tv_sec = (tv)->tv_sec;                            \
                (ts)->tv_nsec = (tv)->tv_usec * 1000;                   \
        } while (0)
#define TIMESPEC_TO_TIMEVAL(tv, ts)                                     \
        do {                                                            \
                (tv)->tv_sec = (ts)->tv_sec;                            \
                (tv)->tv_usec = (ts)->tv_nsec / 1000;                   \
        } while (0)
