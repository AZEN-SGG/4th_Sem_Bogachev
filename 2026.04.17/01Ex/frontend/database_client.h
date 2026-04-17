#ifndef DATABASE_CLIENT_H
#define DATABASE_CLIENT_H

#include "net_status.h"
#include "client.h"

class database_client : public client
{
public:
	net_status query_handler (int fd);
};

#endif // DATABASE_CLIENT_H
