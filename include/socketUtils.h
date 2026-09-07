#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <sys/epoll.h>
#include <arpa/inet.h>
#include <unistd.h>

#define ERROR_FD -1
#define MAX_EVENTS 10
#define READ_SIZE 1000
#define SERVER_IP_ADDRESS "127.0.0.1"
#define SERVER_PORT 9972

struct arg_struct{
    int32_t arg1;
    int32_t arg2;
    struct sockaddr_in arg3;
};

/**
 * Create a basic listening socket
 * 
 * @return fd of the new socket, ERROR_FD if an error accured
 */
int32_t create_listen_socket(struct sockaddr_in address, char* server_ip_address, 
    int32_t server_port);

/**
 * Handle a new socket connection. 
 * Accept a connection to 'socket_fd' and add the new connection socket fd the 'epoll_fd'
 * 
 * @return False in case of failure, otherwise True
 */
bool handle_new_connnection(int32_t epoll_fd, int32_t socket_fd, struct sockaddr_in address);

/**
 * Print incoming messages from epoll instance
 */
void print_messages(int32_t epoll_fd);

void *listen_for_users(void* arguments);