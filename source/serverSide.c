#include "../include/serverSide.h"
#include "../include/socketUtils.h"

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <pthread.h>

#include <sys/epoll.h>

#define INVALID_FD -1

int main()
{
    int32_t epoll_fd = 0; 
    int32_t server_fd = 0;

    // Creating an epoll instance
    epoll_fd = epoll_create1(0);
    if (epoll_fd == INVALID_FD)
    {
        fprintf(stderr, "Failed to create epoll file descriptor");
        return 1;
    }

    struct sockaddr_in address;
    server_fd = create_listen_socket(address, SERVER_IP_ADDRESS, SERVER_PORT);
    if (server_fd < 0)
    {
        close(epoll_fd);
        return 1;
    }

    struct arg_struct args;
    args.arg1 = epoll_fd;
    args.arg2 = server_fd;
    args.arg3 = address;
    pthread_t add_users_thread;
    pthread_create(&add_users_thread, NULL, &listen_for_users, (void *)&args);

    while(true)
    {
        print_messages(epoll_fd);
    }

    close(server_fd);
    close(epoll_fd);
    return 0;
}
