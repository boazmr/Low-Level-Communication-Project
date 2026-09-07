#include "../include/socketUtils.h"

int32_t create_listen_socket(struct sockaddr_in address, char* server_ip_address, int32_t server_port)
{
    int32_t opt = 1;
    int32_t socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd < 0)
    {
        fprintf(stderr, "Socket failure");
        return ERROR_FD;
    }
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        fprintf(stderr, "Set-socket-opt error\n");
        return ERROR_FD;
    }
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr(server_ip_address);
    address.sin_port = htons(server_port);
    
    if(bind(socket_fd, (struct sockaddr*)&address, sizeof(address)) < 0)
    {
        fprintf(stderr, "Bind error\n");
        return ERROR_FD;
    }
    if(listen(socket_fd, 3) < 0)
    {
        fprintf(stderr, "Listen error\n");
        return ERROR_FD;
    }
    
    return socket_fd;
}

int32_t accept_connection(int32_t socket_fd, struct sockaddr_in address)
{
    socklen_t addrlen = sizeof(address);
    int32_t new_socket = accept(socket_fd, (struct sockaddr*)&address, &addrlen);
    if(new_socket < 0)
    {
        fprintf(stderr, "Accept error\n");
        return ERROR_FD;
    }
    return new_socket;
}

bool epoll_add(int32_t epoll_fd, int32_t new_fd)
{
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = new_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, new_fd, &event) == -1)
    {
        fprintf(stderr, "Failed to socket fd to epoll\n");
        return false;
    }
    return true;
}

bool handle_new_connnection(int32_t epoll_fd, int32_t socket_fd, struct sockaddr_in address)
{
    int32_t new_socket = accept_connection(socket_fd, address);
    if(new_socket < 0)
    {
        return false;
    }
    
    if(!epoll_add(epoll_fd, new_socket))
    {
        return false;
    }
    return true;
}

void *listen_for_users(void* arguments)
{
    printf("listening to users\n");
    struct arg_struct *args = arguments;
    int32_t epoll_fd = args -> arg1;
    int32_t server_fd = args -> arg2;
    struct sockaddr_in address = args -> arg3;

    while(true)
    {
        if(!handle_new_connnection(epoll_fd, server_fd, address))
        {
            close(server_fd);
            close(epoll_fd);
            return NULL;
        }
    }
}

void print_messages(int32_t epoll_fd)
{
    int event_count = 0;
    int i = 0;
    int bytes_read = 0;
    char buffer[READ_SIZE + 1] = { 0 };
    struct epoll_event events[MAX_EVENTS];

    event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    for(i = 0; i < event_count; i++)
    {
        printf("Reading file desctriptor %d -- \n", events[i].data.fd);
        bytes_read = read(events[i].data.fd, buffer, READ_SIZE);
        buffer[bytes_read] = '\0';
        if(bytes_read == 0)
        {
            if(epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, NULL))
            {
                fprintf(stderr, "Error removing fd from epoll intrest list!\n");
                return;
            }
        }
        printf("%d bytes read.\n", bytes_read);
        printf("%s", buffer);
    }
}
