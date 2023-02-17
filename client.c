#include "utils.h"

// Algorithm of backoff
int exponential_backoff(int current_waiting_time)
{
    return MIN(current_waiting_time * WAITING_TIME_MULTIPLIER, MAX_WAITING_INTERVAL * 1000);
}

int jitter_backoff(int current_waiting_time)
{
    return randint(MIN(current_waiting_time * WAITING_TIME_MULTIPLIER, MAX_WAITING_INTERVAL * 1000) - BASE_WAITING_TIME_MS + 1) + BASE_WAITING_TIME_MS;
}

int equal_jitter_backoff(int current_waiting_time)
{
    int tmp = MIN(current_waiting_time * WAITING_TIME_MULTIPLIER, MAX_WAITING_INTERVAL * 1000);
    return MIN(tmp / 2 + randint(tmp / 2), MAX_WAITING_INTERVAL * 1000);
}

int decorrelated_jitter_backoff(int current_waiting_time)
{
    return MIN(randint(current_waiting_time * 3 - BASE_WAITING_TIME_MS + 1) + BASE_WAITING_TIME_MS, MAX_WAITING_INTERVAL * 1000);
}

void *receiver(void *arg)
{
    shared_args *args = (shared_args *)arg;
    char buffer[BUFFER_LEN] = {0};
    int client_address_len = sizeof(struct sockaddr_in);
    struct sockaddr_in client_address;

    while (!args->is_end)
    {
        int len = recvfrom(args->sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_address, (socklen_t *)&client_address_len);
        if (len > 0)
        {
            // Received the echo from server, set status to end
            pthread_mutex_lock(&args->mutex);
            args->is_end = true;
            pthread_mutex_unlock(&args->mutex);

            log_info("Received '%s' from server %s", buffer, inet_ntoa(client_address.sin_addr));
            log_info("Receiver early return.");
            return (void *)false;
        }
        buffer[len] = '\0';
        log_info("Receiver timeout and receive nothing from server yet, len = %d", len);
    }
    log_info("Receiver finished.");
    return (void *)true;
}

void *sender(void *arg)
{
    // Set config
    sender_args *args = (sender_args *)arg;
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(args->port);
    server_address.sin_addr.s_addr = inet_addr(args->host);

    // Copy msg into buffer and ready to send
    char buffer[BUFFER_LEN] = {0};
    strcpy(buffer, args->msg);

    int try_times = 0;
    int waiting_time = args->base_waiting_time_ms;
    while(try_times++ < args->max_retry)
    {
        if (args->args->is_end) return NULL;
         
        int len = sendto(args->args->sock, buffer, strlen(buffer), 0, (struct sockaddr *)&server_address, sizeof(server_address));
        log_info("Sent %d bytes: %s, try_times: %d", len, buffer, try_times);
        log_info("Waiting_time is %d ms, start to wait\n", waiting_time);

        msleep(waiting_time);
        waiting_time = args->alg(waiting_time);
    }

    // The sender finished. Set status to end.
    log_info("Sender finished");
    pthread_mutex_lock(&args->args->mutex);
    args->args->is_end = true;
    pthread_mutex_unlock(&args->args->mutex);
    return NULL;
}

shared_args init_shared_args(int sock, bool is_end)
{
    shared_args args = {
        .sock = sock,
        .is_end = is_end,
        PTHREAD_MUTEX_INITIALIZER
    };
    return args;
}

sender_args init_sender_args(shared_args *rec_args, char **argv)
{
    sender_args args = {
        .args = rec_args,
        .host = argv[1],
        .port = atoi(argv[2]),
        .msg = argv[3],
        .base_waiting_time_ms = BASE_WAITING_TIME_MS,
        .max_retry = atoi(argv[4])
    };
    // exponential, jitter, equal_jitter, decorrelated_jitter
    char algo[50] = {0};
    if (!strcmp(argv[5], "exponential"))
    {
        strcpy(algo, "exponential");
        args.alg = exponential_backoff;
    }
    else if (!strcmp(argv[5], "jitter"))
    {
        strcpy(algo, "jitter");
        args.alg = jitter_backoff;
    }
    else if (!strcmp(argv[5], "equal_jitter"))
    {
        strcpy(algo, "equal_jitter");
        args.alg = equal_jitter_backoff;
    }
    else if (!strcmp(argv[5], "decorrelated_jitter"))
    {
        strcpy(algo, "decorrelated_jitter");
        args.alg = decorrelated_jitter_backoff;
    }
    else
    {
        strcpy(algo, "exponential");
        args.alg = exponential_backoff;
    }
    log_info("Algorithm: %s", algo);
    return args;
}

int main(int argc, char *argv[])
{
    // create a UDP socket, return -1 on failure
    int sock;
    void *ret;

    if (argc < 6)
    {
        printf("Usage: %s <server_ip> <port> <message> <max_retry> <retry_algorithm>\n", argv[0]);
        printf("retry_algorithm: exponential, jitter, equal_jitter, decorrelated_jitter\n");
        return 1;
    }

    if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    {
        log_error("could not create socket");
        return 1;
    }

    // shared_args rec_args = {sock, isEnd, PTHREAD_MUTEX_INITIALIZER};
    shared_args rec_args = init_shared_args(sock, false);
    // sender_args send_args = {&rec_args, argv[1], atoi(argv[2]), argv[3], BASE_WAITING_TIME_MS, atoi(argv[4])};
    sender_args send_args = init_sender_args(&rec_args, argv);
    
    
    pthread_t t_receiver, t_sender;
    setTimeOut(&sock, RECEIVER_TIMEOUT, 0, SO_RCVTIMEO);
    setTimeOut(&sock, 0, 0, SO_SNDTIMEO);
    pthread_create(&t_receiver, NULL, receiver, &rec_args);
    pthread_create(&t_sender, NULL, sender, &send_args);
    pthread_join(t_receiver, &ret);
    pthread_join(t_sender, NULL);
    pthread_mutex_destroy(&rec_args.mutex);
    close(sock);
    
    if (ret)
        log_info("Reach max-retry, return %d", ret);
    else
        log_info("The echo message is received, return %d", ret);

    return ret? 1: 0;
}
