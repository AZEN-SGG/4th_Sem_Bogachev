#include "solve.h"
#include "database.h"


io_status solve (char *filename, int *r)
{
	auto db = new (std::nothrow) database();
    if (db == nullptr)
		return io_status::memory;

	io_status ret = db->read_file(filename);
	if (ret != io_status::success)
	{
		delete db;
		return ret;
	}

	ret = db->start(r);
	
	delete db;
	return ret;
}

