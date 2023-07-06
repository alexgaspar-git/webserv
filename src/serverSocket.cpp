
#include "serverSocket.hpp"

serverSocket::serverSocket(int port) {
	srvSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (srvSocket == -1 ) {
		std::cerr << "Failed to create socket. errno: " << errno << std::endl;
		exit(1);//doit t'on vraiment utiliser exit
	}
	if (fcntl(srvSocket, F_SETFL, O_NONBLOCK) == -1) {
		std::cout << "fcntl failed. errno: " << errno << std::endl;
		close(srvSocket);
		exit(1);
	}
	srvAdress.sin_family = AF_INET;
    srvAdress.sin_addr.s_addr = INADDR_ANY;
    srvAdress.sin_port = htons(port);
	addrlen = sizeof(srvAdress);
	if (bind(srvSocket, (sockaddr *)&srvAdress, addrlen) == -1) {
		std::cerr << "failed to bind server socket. errno: " << errno << std::endl;
		close(srvSocket);
		exit(1);
	}
	if (listen(srvSocket, 10) == -1) {
		std::cerr << "failed to listen on server socket. errno: " << errno << std::endl;
		close(srvSocket);
		exit(1);
	}
}

serverSocket::serverSocket(serverSocket const &rhs) {
	*this = rhs;
}//brouillon

serverSocket::~serverSocket() {
	close(srvSocket);
}

serverSocket &serverSocket::operator=(serverSocket const &rhs) {//brouillon
	this->srvSocket = rhs.srvSocket;
	this->srvAdress = rhs.srvAdress;
	this->addrlen = rhs.addrlen;
	return (*this);
}
