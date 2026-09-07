#include <stdio.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>

#include "include/serverSide.h"

#define STDIN_FD 0
#define INVALID_FD -1

#define MAX_EVENTS 5
#define READ_SIZE 10

int main()
{
    bool running = true;
    int32_t event_count = 0;
    int32_t i = 0;
    size_t bytes_read = 0;
    char read_buffer[READ_SIZE + 1];

    struct epoll_event event;
    struct epoll_event events[MAX_EVENTS];
    int32_t epoll_fd = epoll_create1(0);

    // Check epoll create return value
    if(epoll_fd == INVALID_FD)
    {
        fprintf(stderr, "Failed to create epoll file descriptor\n");
    }

    event.events = EPOLLIN;
    event.data.fd = STDIN_FD;

    if(epoll_ctl(epoll_fd, EPOLL_CTL_ADD, STDIN_FD, &event))
    {
        fprintf(stderr, "Failed to add file descriptor to epoll\n");
        close(epoll_fd);
        return 1;
    }

    // Open the file for read
    int32_t output_fd = open("epoll_output.txt", O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR);
    if(output_fd == INVALID_FD)
    {
        fprintf(stderr, "Failed to open output file\n");
        close(epoll_fd);
        return 1;
    }

    while(running)
    {
        printf("Polling for input\n");
        event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, 30000);
        printf("%d ready events\n", event_count);
        for(i = 0; i < event_count; i++)
        {
            printf("Reading file desctriptor %d -- ", events[i].data.fd);
            bytes_read = read(events[i].data.fd, read_buffer, READ_SIZE);
            printf("%zd bytes read.\n", bytes_read);
            read_buffer[bytes_read] = '\0';
            printf("Read %s\n", read_buffer);
            if(write(output_fd, read_buffer, bytes_read - 1) == -1)
            {
                fprintf(stderr, "Invalid write to output file\n");
                running = false;
            }

            if (!strncmp(read_buffer, "stop\n", 5))
            {
                running = false;
            }
        }
    }

    if(close(epoll_fd))
    {
        fprintf(stderr, "Failed to close epoll file descriptor\n");
    }
    if(close(output_fd))
    {
        fprintf(stderr, "Failed to close output file descriptor\n");
    }

    return 0;
}