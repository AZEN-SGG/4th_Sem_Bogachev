#include "solve.h"
#include "net_status.h"


net_status solve (char *addr, int port)
{
	net_status ret;
	auto cl = new (std::nothrow) database_client();
    if (cl == nullptr)
		return net_status::memory;

	if ((ret = cl->setup()) != net_status::success)
	{
		delete cl;
		return ret;
	}

	ret = cl->run(addr, port);
	
	delete cl;
	return ret;
}

