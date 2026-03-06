#include "solve.h"
#include "io_status.h"
#include "request.h"


io_status start_db (const list2<record> *db, int *res)
{
	io_status ret;
	char buf[LEN] = {};
	request x;
	FILE *f_out = stdout,
		 *f_in = stdin;

	(*res) = 0;

	while (fgets(buf, LEN, f_in))
	{
		if ((ret = x.parse(buf)) == io_status::format)
			fprintf(f_out, "Wrong format of request!\n\n");
		else if (ret != io_status::success)
			return ret;
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

