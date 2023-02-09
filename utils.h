#ifndef UTIL_H
#define UTIL_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include "logging/src/log.h"

#define BUFFER_LEN 512
#define WAITING_TIME_MULTIPLIER 2
#define MAX_WAITING_INTERVAL 8
#define BASE_WAITING_TIME_MS 500
#define MIN(a,b) ((a<b)?a:b)
#define RECEIVER_TIMEOUT 5

typedef struct {
    int sock;
    volatile bool is_end;
    pthread_mutex_t mutex;
} shared_args;

typedef int (*backoff_alg)(int current_waiting_time);
typedef struct {
    shared_args* args;
    char* host;
    int port;
    char* msg;
    int base_waiting_time_ms;
    int max_retry;
    backoff_alg alg;
} sender_args;

int msleep(long msec);

int randint(int n);

void setTimeOut(int* sock, int sec, int usec, int type);


#endif
