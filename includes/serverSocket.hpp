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
		int srvSocket;
		sockaddr_in srvAdress;
		socklen_t	addrlen;
		int kqueue_fd;
		struct kevent event;

	public:

		serverSocket(int port);
		serverSocket(serverSocket const &src);
		~serverSocket();
		
		void init_kqueue();
		void create_request();
		void handle_request(int clientSocket);

		int				getSrvSocket() const;
		int				getKqueue_fd() const;
		sockaddr_in		getSrvAdress() const;
		socklen_t		getAddrlen() const;

		serverSocket &operator=(serverSocket const &rhs);
		int CreateSocket(ConfigParser *pars);
};

#endif
