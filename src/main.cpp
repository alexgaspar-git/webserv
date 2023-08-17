#include "../includes/serverSocket.hpp"
#include "../includes/requestHandler.hpp"
#include "../includes/ConfigParser.hpp"

int main(int argc, char **argv) {
	if (argc > 2) {
		std::cerr << "./webserv [conf file]" << std::endl;
		return 1;
	}
	std::string name_conf;
	if (argc == 2)
		name_conf = argv[1];
	else
		name_conf = "conf/example.conf";
	std::ifstream input(name_conf);
	if (!input.is_open()) {
		std::cout << ".conf file doesn't exist." << std::endl;
		return (1);
	}
	std::string conf((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
	ConfigParser pars;
	if (pars.check_conf(conf))
		return (1);
	int new_events;
	serverSocket srv;
    srand(time(NULL));
	if (srv.CreateSocket(&pars))
		return (1);
	struct kevent *events = srv.event;
	while (1) {
		int tmp_fd;
		new_events = kevent(srv.getKqueue_fd(), NULL, 0, events, 50, NULL);
		for (int i = 0; i < new_events; i++) {
			tmp_fd = events[i].ident;
			if (srv._srvSocket->find(tmp_fd) != srv._srvSocket->end())
				srv.create_request(tmp_fd);
			else
				srv.handle_request(tmp_fd, &pars);
		}
	}
	return (0);
}