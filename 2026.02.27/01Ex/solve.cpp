#include "solve.h"
#include <cstdio>


io_status start_db (const list2<record> *db, int *res)
{
	char buf[LEN] = {};
	command x;
	FILE *f_out = stdout,
		 *f_in = stdin;

	(*res) = 0;

	while (fgets(buf, LEN, f_in))
	{
		if (!x.parse(buf))
			fprintf(f_out, "Wrong format of request!\n\n");
		else
			(*res) += db->print_valid(x, f_out);
	}

	if (feof(f_in) == 0)
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

	ret = start_db(shai_hulud.get(), r);
	
	return ret;
}

