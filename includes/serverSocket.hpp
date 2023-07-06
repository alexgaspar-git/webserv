
#ifndef SERVERSOCKET_HPP
 #define SERVERSOCKET_HPP

# include <iostream>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <fcntl.h>
# include <unistd.h>

class serverSocket
{
	private:

	public:
		int srvSocket;
		sockaddr_in srvAdress;
		socklen_t	addrlen;

		serverSocket(int port);
		serverSocket(serverSocket const &src);
		~serverSocket();

		serverSocket &operator=(serverSocket const &rhs);
};

#endif
