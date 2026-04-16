#include "solve.h"
#include "database_manager.h"
#include "io_status.h"
#include <ctime>


io_status solve (char *path, char *filename, int port, int *r, double *time)
{
	io_status ret;
	auto db = new (std::nothrow) database_manager();
    if (db == nullptr)
		return io_status::memory;

	if (db->setup(port))
	{
		delete db;
		return io_status::network;
	}

	ret = db->read_file(filename);
	if (ret != io_status::success)
	{
		delete db;
		return ret;
	}

	(*time) = clock();
	ret = db->run(path, r);
	(*time) = (clock() - *time) / CLOCKS_PER_SEC;
	
	delete db;
	return ret;
}

