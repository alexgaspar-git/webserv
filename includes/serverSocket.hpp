#ifndef SERVERSOCKET_HPP
 #define SERVERSOCKET_HPP

# include <iostream>
# include <sys/socket.h>
# include <sys/types.h>
# include <sys/event.h>
# include <netinet/in.h>
# include <fcntl.h>
# include <unistd.h>
# include "requestHandler.hpp"
# include "ConfigParser.hpp"

class serverSocket
{
	private:
		int srvskt;
		sockaddr_in srvAdress;
		socklen_t	addrlen;
		int kqueue_fd;

	public:
		struct kevent event[50];
		std::map<int, std::string> *_srvSocket;

		serverSocket();
		~serverSocket();
		
		int init_kqueue();
		int CreateSocket(ConfigParser *pars);
		void create_request(int fd);
		void handle_request(int clientSocket, ConfigParser *pars);
		void close_all();

		int				getsrvskt() const;
		int				getKqueue_fd() const;
		sockaddr_in		getSrvAdress() const;
		socklen_t		getAddrlen() const;

};

#endif