#ifndef CLIENT_H
#define CLIENT_H

#include "net_status.h"

#include <cstdio>
#include <string.h>
#include <errno.h>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

class client
{
private:
	int sock = -1;
public:
	client () = default;
	virtual ~client () { erase(); }

	net_status setup ();
	net_status run (char *addr, int port);

	// Интерфейс
	virtual net_status query_handler (int fd) = 0;
protected:
	net_status write_server(int fd, char *buf, int size);
	net_status read_server(int fd, FILE *fp);
private:
	void erase ()
	{
		if (sock >= 0)
			close(sock);
	}
};

#endif // CLIENT_H

