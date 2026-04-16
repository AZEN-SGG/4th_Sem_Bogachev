#ifndef SERVER_H
#define SERVER_H

#include "io_status.h"

#include <ctime>
#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <cstddef>
#include <string.h>
#include <ctype.h>
#include <sys/select.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

class server
{
private:
	int sock = -1;
	int port = 1505;
	int max_sock = 0;
	int timeout = 60; // Timeout через минуту
	struct sockaddr_in addr = {};
	fd_set active_set;
public:
	server () = default;
	virtual ~server () { erase(); }

	int setup (const int p = 1505, const int t = 60);
	io_status run (char *path, int *res); 
	
	virtual io_status query_handler(char *buf, int *res, FILE *fp) = 0;
private:
	void erase ()
	{
		port = 1505;
		addr = {};

		FD_CLR(sock, &active_set);
		if (sock > 0)
			close(sock);

		for (int i = 0 ; i < max_sock + 1 ; ++i)
			if (FD_ISSET(i, &active_set))
				close(i);
		
		FD_ZERO(&active_set);
		sock = 0;
		max_sock = 0;
	}
	int read_fd(int fd, char *buf);
};

#endif // SERVER_H
