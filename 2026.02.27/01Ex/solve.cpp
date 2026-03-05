#include "solve.h"


io_status start_db (const list2<record> *db)
{
	char buf[LEN] = {};
	command x;

	while (fgets(buf, LEN, stdin))
	{
		if (!x.parse(buf))
			return io_status::format;
	}

	if (feof(stdin) == 0)
		return io_status::read;

	return io_status::success;
}


io_status solve (char *filename, int *r)
{
	auto shai_hulud = std::make_unique<list2<record>>();
    if (shai_hulud == nullptr)
		return io_status::memory;

	io_status ret = shai_hulud->read_file(filename);
	if (ret != io_status::success)
		return ret;

	shai_hulud->print();

	(*r) = 0;

	ret = io_status::success;
	
	return ret;
}

