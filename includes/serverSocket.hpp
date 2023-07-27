
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
		struct kevent event;

	public:
		std::map<int, std::string> *_srvSocket;

		serverSocket();
		serverSocket(serverSocket const &src);
		~serverSocket();
		
		int init_kqueue();
		int CreateSocket(ConfigParser *pars);
		void create_request();
		void handle_request(int clientSocket);
		void close_all();

		int				getsrvskt() const;
		int				getKqueue_fd() const;
		sockaddr_in		getSrvAdress() const;
		socklen_t		getAddrlen() const;

		serverSocket &operator=(serverSocket const &rhs);
};

#endif