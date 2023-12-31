
#include "../includes/serverSocket.hpp"

serverSocket::serverSocket() {
	this->_srvSocket = new std::map<int, std::string>;
}

serverSocket::~serverSocket() {
	close_all();
	delete this->_srvSocket;
}

int translate_port(std::string port) {
	int i = 0;
	for ( std::string::iterator it=port.begin(); it!=port.end(); ++it) {
		if (!isdigit(port[i++])) {
			std::cerr<< "only digit are expted for ports" << std::endl;
			return (0);
		}
	}
	std::istringstream iss(port);
	int number;
	iss >> number;
	if (number < 1024 || number > 49151)
		return (0);
	return (number);
}

void serverSocket::close_all() {
	for (std::map<int, std::string>::iterator it = this->_srvSocket->begin(); it != this->_srvSocket->end(); it++) {
		close(it->first);
	}
	close(this->kqueue_fd);
}

int serverSocket::CreateSocket(ConfigParser *pars) {
	int port;
	if ((this->kqueue_fd = kqueue()) == -1) {
		std::cerr << "failed to create kqueue." << std::endl;
		return(1);
	}
	for (std::vector<s_conf>::iterator it = pars->_config->begin(); it != pars->_config->end(); it++) {
		if (!(port = translate_port(it->port))) {
			std::cout << "valid ports are between 1024 and 49151" << std::endl;
			return (1);
		}
		this->srvskt = socket(AF_INET, SOCK_STREAM, 0);
		if (this->srvskt == -1 ) {
			std::cerr << "Failed to create socket." << std::endl;
			return (1);
		}
		if (fcntl(this->srvskt, F_SETFL, O_NONBLOCK) == -1) {
			std::cerr << "fcntl failed." << std::endl;
			close(this->srvskt);
			return (1);
		}
		this->srvAdress.sin_family = AF_INET;
		this->srvAdress.sin_addr.s_addr = INADDR_ANY;
		this->srvAdress.sin_port = htons(port);
		this->addrlen = sizeof(this->srvAdress);
		if (bind(this->srvskt, (sockaddr *)&this->srvAdress, this->addrlen) == -1) {
			std::cerr << "failed to bind server socket." << std::endl;
			close(this->srvskt);
			return (1);
		}
		if (listen(this->srvskt, 50) == -1) {
			std::cerr << "failed to listen on server socket." << std::endl;
			close(this->srvskt);
			return (1);
		}
		EV_SET(this->event, this->srvskt, EVFILT_READ, EV_ADD, 0, 0, NULL);
		if (kevent(this->kqueue_fd, this->event, 1, NULL, 0, NULL) == -1) {
			std::cerr << "failed to register event." << std::endl;
			close(this->srvskt);
			return(1);
		}
		this->_srvSocket->insert(std::pair<int, std::string>(this->srvskt, it->port));
	}
	std::cout << "Server up" << std::endl;
	return (0);
}

void serverSocket::create_request(int fd) {
	int clientSocket;
	sockaddr_in clientAdress;
	socklen_t cli_addrlen = sizeof(clientAdress);
	clientSocket = accept(fd, reinterpret_cast<sockaddr *>(&clientAdress), &cli_addrlen);
	if (clientSocket == -1) {
		std::cerr << "failed to accept connection." << std::endl;
		return;
	}
	if (fcntl(clientSocket, F_SETFL, O_NONBLOCK) == -1) {
		std::cerr << "fcntl failed." << std::endl;
		close(clientSocket);
		return;
	}
	EV_SET(this->event, clientSocket, EVFILT_READ, EV_ADD, 0, 0, NULL);
	if (kevent(this->kqueue_fd, this->event, 1, NULL, 0, NULL) == -1) {
		std::cerr << "failed to register event." << std::endl;
		close(clientSocket);
		return;
	}
}

void serverSocket::handle_request(int clientSocket, ConfigParser *pars) {
	std::string request;
    size_t bytesRead = 0;
    char buf[1024];
    for (size_t nb = 1; nb != 0 && nb != SIZE_T_MAX;) {
        memset(buf, 0, sizeof(buf));
        nb = recv(clientSocket, buf, sizeof(buf) - 1, MSG_DONTWAIT);
		if (nb == SIZE_T_MAX || bytesRead > SIZE_T_MAX/2) {
			bytesRead = -1;
			break;
		}
		else
			bytesRead += nb;
		request += std::string(buf, nb);
	}
	if (bytesRead > 0) {
		int port;
		if (getsockname(clientSocket, (struct sockaddr*)&srvAdress, &addrlen) == 0) {
			port = ntohs(srvAdress.sin_port);
			requestHandler req(request, pars, port);
			if (!req._noCurrentClient) {
				std::string response = req.handleRequest();
				if (write(clientSocket, response.c_str(), response.length()) <= 0)
					std::cerr << "write didn't work properly" <<std::endl;
			}
		}
		else
			std::cerr << "could not get port" << std::endl;
	}
	EV_SET(this->event, clientSocket, EVFILT_READ, EV_DELETE, 0, 0, NULL);
	int resultDelete = kevent(this->kqueue_fd, this->event, 1, NULL, 0, NULL);
	if (resultDelete == -1) {
		std::cerr << "failed to delete event." << std::endl;
	}
	close(clientSocket);
}

int serverSocket::getsrvskt() const {
	return(this->srvskt);
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
