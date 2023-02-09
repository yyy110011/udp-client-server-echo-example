#include "utils.h"

int msleep(long msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;

    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

// Generate uniform random interger between 0 to n
int randint(int n)
{
    srand((unsigned)time(NULL));
    if ((n - 1) == RAND_MAX)
    {
        return rand();
    } 
    else
    {
        // calculate the length of integer could be divided
        long end = RAND_MAX / n;
        assert (end > 0L);
        end *= n;
        // remove the number out of limit
        int r;
        while ((r = rand()) >= end);
        return r % n;
    }
}

void setTimeOut(int* sock, int sec, int usec, int type)
{
    struct timeval tv;
    tv.tv_sec = sec;
    tv.tv_usec = usec;
    setsockopt(*sock, SOL_SOCKET, type, (const char*)&tv, sizeof tv); 
}
