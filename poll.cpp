#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdlib>
#include <fcntl.h>
#include <netinet/in.h>

int main(int argc, char const *argv[])
{
    int server_fd, new_socket; long valread;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    
   char response[74] = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
    
    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        std::cout << "Failed to create socket. errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);
    
    memset(address.sin_zero, '\0', sizeof address.sin_zero);// checker comment remplacer
    
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        std::cout << "Failed to bind to port 8080. errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 10) < 0)// checker le nombre de socket a attendre
    {
        std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
        exit(EXIT_FAILURE);
    }

    fcntl(server_fd, F_SETFL, O_NONBLOCK);

    while(1)
    {
        std::cout << std::endl << "+++++++ Waiting for new connection ++++++++" << std::endl << std::endl;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0)
        {
            std::cout << "Failed to accept connection. errno: " << errno << std::endl;
            exit(EXIT_FAILURE);
        }
        // a remplacer ->
        char buffer[30000] = {0};
        valread = read( new_socket , buffer, 30000);
        printf("%s",buffer );
        write(new_socket , response , strlen(response));
        printf("------------------Hello message sent-------------------\n");
        // <-
        close(new_socket);
    }
    return 0;
}