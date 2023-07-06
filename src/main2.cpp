#include "../includes/serverSocket.hpp"
#include "../includes/requestHandler.hpp"

int main() {
	int new_events;
	serverSocket srv(8080);
	srv.init_kqueue();

	while (1) {
		int tmp_fd;
		struct kevent events[10];
		new_events = kevent(srv.getKqueue_fd(), NULL, 0, events, 10, NULL);//protection kevent ?
		for (int i = 0; i < new_events; i++) {
			tmp_fd = events[i].ident;
			if (tmp_fd == srv.getSrvSocket())
				srv.create_request();
			else
				srv.handle_request(tmp_fd);
		}
	}
	close(srv.getKqueue_fd());
	close(srv.getSrvSocket());
	return (0);
}