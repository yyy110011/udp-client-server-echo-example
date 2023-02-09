#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "logging/src/log.h"

#define BUFFER_LEN 512  //Max length of buffer
#define PORT 8888   //The port on which to listen for incoming data


bool is_valid_input(const char *input)
{
    return strcmp(input, "0") == 0 || strcmp(input, "1") == 0;
}

int main(int argc, char *argv[])
{
    struct sockaddr_in server_address, clien_address;
    int s, i, slen = sizeof(clien_address);
    char buffer[BUFFER_LEN];
    bool enableEcho = true;
    log_set_level(LOG_INFO);

    if (argc < 2)
    {
        log_info("Usage: %s --Echo 0/1", argv[0]);
        log_info("enableEcho = 1 (default)");
    }
    else if (argc == 3 && strcmp(argv[1], "--Echo") == 0)
    {
        if (!is_valid_input(argv[2]))
        {
            log_error("Error: Invalid input for --Echo option. Value must be either 0 or 1.");
            return 1;
        }
        enableEcho = atoi(argv[2]);
        log_info("enableEcho = %d", enableEcho);
    }
    else
    {
        log_error("Error: Invalid usage.");
        log_info("Usage: %s --Echo 0/1", argv[0]);
        return 1;
    }

    //create a UDP socket
    if ((s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        perror("socket");
        return 1;
    }

    // zero out the structure
    memset((char *) &server_address, 0, sizeof(server_address));

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind socket to port
    if( bind(s , (struct sockaddr*)&server_address, sizeof(server_address) ) == -1)
    {
        perror("bind");
        return 1;
    }

    //keep listening for data
    bool mode = true;
    while(1)
    {

        log_info("Listen to the port 8888...");
        fflush(stdout);

        //try to receive some data, this is a blocking call
        int len = recvfrom(s, buffer, BUFFER_LEN, 0, (struct sockaddr *) &clien_address, &slen);
        if (len == -1)
        {
            perror("recvfrom()");
            return 1;
        }
        buffer[len] = '\0';
        //print details of the client/peer and the data received
        log_info("Received packet from %s:%d", inet_ntoa(clien_address.sin_addr), ntohs(clien_address.sin_port));

        log_info("Message from client: '%s'" , buffer);

        //now reply the client with the same data
        if (enableEcho)
        {   
            if (sendto(s, buffer, strlen(buffer), 0, (struct sockaddr*) &clien_address, slen) == -1)
            {
                perror("sendto()");
                return 1;
            }
        }
    }
    return 0;
}
