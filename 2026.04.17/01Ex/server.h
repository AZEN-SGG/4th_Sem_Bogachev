#ifndef SERVER_H
#define SERVER_H

#include "io_status.h"

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
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
	int sock = 0;
	int port = 1505;
	int max_sock = 0;
	int timeout = 60; // Timeout через минуту
	struct sockaddr_in addr;
	fd_set active_set;
public:
	server () = default;

	int setup (const int p = 1505, const int t = 60);
	int run (); 
	
	virtual io_status query_handler(char *buf, FILE *fp) = 0;
};

#endif // SERVER_H
