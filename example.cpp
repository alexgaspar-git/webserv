#include <iostream>
#include <vector>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

const int MAX_EVENTS = 10;
const int BUFFER_SIZE = 1024;

int main() {
    int serverSocket, clientSocket;
    sockaddr_in serverAddress, clientAddress;

    // Create server socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Failed to create server socket" << std::endl;
        return 1;
    }

    // Set up server address
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8080);

    // Bind the server socket
    if (bind(serverSocket, (sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        std::cerr << "Failed to bind server socket" << std::endl;
        return 1;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 10) < 0) {
        std::cerr << "Failed to listen on server socket" << std::endl;
        return 1;
    }

    // Set server socket to non-blocking mode
    fcntl(serverSocket, F_SETFL, O_NONBLOCK);

    // Create a pollfd structure for server socket
    pollfd serverPoll;
    serverPoll.fd = serverSocket;
    serverPoll.events = POLLIN;

    std::vector<pollfd> clientPolls;
    std::vector<int> clientSockets;

    char buffer[BUFFER_SIZE];

    while (true) {
        // Use poll to wait for events
        std::vector<pollfd> eventPolls;
        eventPolls.push_back(serverPoll);
        for (size_t i = 0; i < clientPolls.size(); i++) {
            eventPolls.push_back(clientPolls[i]);
        }

        if (poll(&eventPolls[0], eventPolls.size(), -1) == -1) {
            std::cerr << "Failed to poll for events" << std::endl;
            break;
        }

        // Check for server socket events
        if (eventPolls[0].revents & POLLIN) {
            socklen_t clientAddressLength = sizeof(clientAddress);

            // Accept new connection
            clientSocket = accept(serverSocket, (sockaddr *)&clientAddress, &clientAddressLength);
            if (clientSocket == -1) {
                std::cerr << "Failed to accept connection" << std::endl;
                continue;
            }
            char buffer[30000] = {0};
            int valread = read( clientSocket , buffer, 30000);
            printf("%s",buffer);
            // Set client socket to non-blocking mode
            fcntl(clientSocket, F_SETFL, O_NONBLOCK);

            // Create pollfd structure for client socket
            pollfd clientPoll;
            clientPoll.fd = clientSocket;
            clientPoll.events = POLLIN | POLLOUT;

            // Add client socket to the poll list
            clientPolls.push_back(clientPoll);
            clientSockets.push_back(clientSocket);
        }

        // Check for client socket events
        for (size_t i = 0; i < clientPolls.size(); i++) {
            if (eventPolls[i + 1].revents & (POLLIN | POLLOUT)) {
                // Read event
                if (eventPolls[i + 1].revents & POLLIN) {
                    int bytesRead = recv(clientSockets[i], buffer, BUFFER_SIZE, 0);
                    if (bytesRead <= 0) {
                        // Connection closed by client or error occurred
                        close(clientSockets[i]);
                        clientPolls.erase(clientPolls.begin() + i);
                        clientSockets.erase(clientSockets.begin() + i);
                        continue;
                    }

                    // Process received data
                    std::cout << "Received: " << std::string(buffer, bytesRead) << std::endl;
                }

                // Write event
                if (eventPolls[i + 1].revents & POLLOUT) {
                    // Send response to the client
                    std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
                    int bytesSent = send(clientSockets[i], response.c_str(), response.size(), 0);
                    if (bytesSent <= 0) {
                        // Connection closed by client or error occurred
                        close(clientSockets[i]);
                        clientPolls.erase(clientPolls.begin() + i);
                        clientSockets.erase(clientSockets.begin() + i);
                        continue;
                    }

                    // Response sent successfully
                    close(clientSockets[i]);
                    clientPolls.erase(clientPolls.begin() + i);
                    clientSockets.erase(clientSockets.begin() + i);
                }
            }
        }
    }

    // Close server socket
    close(serverSocket);

    return 0;
}
