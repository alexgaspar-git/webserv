
#include "../includes/serverSocket.hpp"

serverSocket::serverSocket(int port) {//donner en arg config_parser pour recuperer tout les ports
//creer std::map<int, std::string>(fd, port)
//creer boucle while qui listen bind plusieur socket au meme kqueue
	this->srvSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (this->srvSocket == -1 ) {
		std::cerr << "Failed to create socket. errno: " << errno << std::endl;
		exit(1);//doit t'on vraiment utiliser exit (la reponse est non)
	}
	if (fcntl(this->srvSocket, F_SETFL, O_NONBLOCK) == -1) {
		std::cout << "fcntl failed. errno: " << errno << std::endl;
		close(this->srvSocket);
		exit(1);
	}
	this->srvAdress.sin_family = AF_INET;
    this->srvAdress.sin_addr.s_addr = INADDR_ANY;
    this->srvAdress.sin_port = htons(port);
	this->addrlen = sizeof(this->srvAdress);
	if (bind(this->srvSocket, (sockaddr *)&this->srvAdress, this->addrlen) == -1) {
		std::cerr << "failed to bind server socket. errno: " << errno << std::endl;
		close(this->srvSocket);
		exit(1);
	}
	if (listen(this->srvSocket, 10) == -1) {
		std::cerr << "failed to listen on server socket. errno: " << errno << std::endl;
		close(this->srvSocket);
		exit(1);
	}
	std::cout << "Server is now listening on port " << port << std::endl;
}

serverSocket::serverSocket(serverSocket const &rhs) {
	*this = rhs;
}//brouillon

serverSocket::~serverSocket() {
	close(this->srvSocket);
}

void serverSocket::init_kqueue() {
	if ((this->kqueue_fd = kqueue()) == -1) {
		std::cerr << "failed to create kqueue. errno: " << errno << std::endl;
		close(this->srvSocket);
		exit(1);
	}
	EV_SET(&this->event, this->srvSocket, EVFILT_READ, EV_ADD, 0, 0, NULL);
	if (kevent(this->kqueue_fd, &this->event, 1, NULL, 0, NULL) == -1) {
		std::cerr << "failed to register event. errno: " << errno << std::endl;
		close(this->srvSocket);
		close(this->kqueue_fd);
		exit(1);
	}
}

void serverSocket::create_request() {
	int clientSocket;
	sockaddr_in clientAdress;
	socklen_t cli_addrlen = sizeof(clientAdress);

	clientSocket = accept(this->srvSocket, reinterpret_cast<sockaddr *>(&clientAdress), &cli_addrlen);
	if (clientSocket == -1) {
		std::cerr << "failed to accept connection. errno: " << errno << std::endl;
		return;
	}
	if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1) {
		std::cout << "fcntl failed. errno: " << errno << std::endl;
		close(clientSocket);
		return;
	}
	EV_SET(&this->event, clientSocket, EVFILT_READ, EV_ADD, 0, 0, NULL);
	if (kevent(this->kqueue_fd, &this->event, 1, NULL, 0, NULL) == -1) {
		std::cerr << "failed to register event. errno: " << errno << std::endl;
		close(clientSocket);
		return;
	}
}

void serverSocket::handle_request(int clientSocket) {
    char buffer[8192];//verifier si suffisant pour la methode post
    ssize_t bytesRead = read(clientSocket, buffer, sizeof(buffer));
    if (bytesRead > 0) {
        std::string request(buffer, bytesRead);
		// std::cout << std::endl;
		// std::cout << "-------- REQUEST RECEIVED --------" << std::endl;
		// std::cout << request;
		// std::cout << "----------------------------------" << std::endl;
        requestHandler test(request);
        std::string response = test.handleRequest();
        write(clientSocket, response.c_str(), response.length());
    }
    close(clientSocket);
}

serverSocket &serverSocket::operator=(serverSocket const &rhs) {//brouillon
	this->srvSocket = rhs.srvSocket;
	this->srvAdress = rhs.srvAdress;
	this->addrlen = rhs.addrlen;
	return (*this);
}

int serverSocket::getSrvSocket() const {
	return(this->srvSocket);
}

sockaddr_in serverSocket::getSrvAdress() const {
	return (this->srvAdress);
}

socklen_t	serverSocket::getAddrlen() const {
	return (this->addrlen);
}

int serverSocket::getKqueue_fd() const {
	return (this->kqueue_fd);
}
