#include "solve.h"
#include "database_manager.h"
#include <ctime>


io_status solve (char *filename, int *r, double *time)
{
	auto db = new (std::nothrow) database_manager();
    if (db == nullptr)
		return io_status::memory;

	io_status ret = db->read_file(filename);
	if (ret != io_status::success)
		return ret;

	(*time) = clock();
	ret = db->start(r);
	(*time) = (clock() - *time) / CLOCKS_PER_SEC;
	
	delete db;
	return ret;
}

