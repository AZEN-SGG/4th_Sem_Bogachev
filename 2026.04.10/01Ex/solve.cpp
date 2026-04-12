#include "solve.h"
#include "database_manager.h"


io_status solve (char *filename, int *r)
{
	auto db = new (std::nothrow) database_manager();
    if (db == nullptr)
		return io_status::memory;

	io_status ret = db->read_file(filename);
	if (ret != io_status::success)
		return ret;

	ret = db->start(r);
	
	delete db;
	return ret;
}

