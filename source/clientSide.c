#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>

#define BUFFER_SIZE 1000

int main()
{
    int status = 0;
    int valread = 0;
    int client_fd = 0;
    int opt = 1;
    struct sockaddr_in address;
    char read_buffer[BUFFER_SIZE + 1] = { 0 };
    int characters_read = 0;

    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0)
    {
        fprintf(stderr, "Socket failure");
        return 1;
    }

    if (setsockopt(client_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        fprintf(stderr, "Set-socket-opt error\n");
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = inet_addr("127.0.0.1");
    address.sin_port = htons(9972);

    status = connect(client_fd, (struct sockaddr*)&address, sizeof(address));
    if(status < 0)
    {
        fprintf(stderr, "Socket connect error\n");
        return 1;
    }

    while(true)
    {
        // Find a way to fix this
        fgets(read_buffer, BUFFER_SIZE, stdin);
        if(strlen(read_buffer) > 0)
        {
            write(client_fd, read_buffer, strlen(read_buffer));
        }
    }

    printf("finishd writing\n");

    close(client_fd);
    return 0;

}