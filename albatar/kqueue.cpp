#include <sys/types.h>
#include <sys/event.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>

const int MAX_EVENTS = 10;
const int PORT = 8080;

void handleClient(int clientSocket) {
    // Read the request from the client
    char buffer[1024];
    ssize_t bytesRead = read(clientSocket, buffer, sizeof(buffer));
    if (bytesRead > 0) {
        std::string request(buffer, bytesRead);
        std::cout << "Received request:\n" << request << std::endl;

        // Send a simple response
        std::string response = "HTTP/1.1 200 OK\r\nContent-Length: 13\r\n\r\nHello, World!";
        write(clientSocket, response.c_str(), response.length());
    }

    // Close the client socket
    close(clientSocket);
}

int main() {
    // Create a socket for incoming connections
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        std::cerr << "Failed to create socket." << std::endl;
        return 1;
    }

    // Bind the socket to a specific port
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(PORT);
    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) < 0) {
        std::cerr << "Failed to bind socket." << std::endl;
        close(serverSocket);
        return 1;
    }

    // Start listening for incoming connections
    if (listen(serverSocket, SOMAXCONN) < 0) {
        std::cerr << "Failed to listen on socket." << std::endl;
        close(serverSocket);
        return 1;
    }

    // Create a kqueue descriptor
    int kq = kqueue();
    if (kq < 0) {
        std::cerr << "Failed to create kqueue." << std::endl;
        close(serverSocket);
        return 1;
    }

    // Register the server socket for reading events
    struct kevent event;
    EV_SET(&event, serverSocket, EVFILT_READ, EV_ADD, 0, 0, nullptr);
    if (kevent(kq, &event, 1, nullptr, 0, nullptr) < 0) {
        std::cerr << "Failed to register event." << std::endl;
        close(serverSocket);
        close(kq);
        return 1;
    }

    std::cout << "Server started. Listening on port " << PORT << std::endl;

    // Wait for events
    struct kevent events[MAX_EVENTS];
    while (true) {
        int numEvents = kevent(kq, nullptr, 0, events, MAX_EVENTS, nullptr);
        if (numEvents < 0) {
            std::cerr << "Failed to wait for events." << std::endl;
            break;
        }

        // Handle each event
        for (int i = 0; i < numEvents; ++i) {
            int fd = events[i].ident;

            // Check if it's the server socket
            if (fd == serverSocket) {
                // Accept the incoming connection
                sockaddr_in clientAddress;
                socklen_t clientAddressLength = sizeof(clientAddress);
                int clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddress), &clientAddressLength);
                if (clientSocket < 0) {
                    std::cerr << "Failed to accept connection." << std::endl;
                    continue;
                }

                // Register the client socket for reading events
                EV_SET(&event, clientSocket, EVFILT_READ, EV_ADD, 0, 0, nullptr);
                if (kevent(kq, &event, 1, nullptr, 0, nullptr) < 0) {
                    std::cerr << "Failed to register event." << std::endl;
                    close(clientSocket);
                    continue;
                }

                std::cout << "New connection from " << inet_ntoa(clientAddress.sin_addr) << std::endl;
            } else {
                // Handle the client socket event
                handleClient(fd);
            }
        }
    }

    // Close the server socket and kqueue descriptor
    close(serverSocket);
    close(kq);

    return 0;
}
