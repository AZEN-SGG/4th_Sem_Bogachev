#ifndef NET_STATUS_H
#define NET_STATUS_H

#define LEN 1234

enum class net_status
{
	success,
	host,
	socket,
	connect,
	write,
	read,
	memory,
};

#endif // NET_STATUS_H
