#include "solve.h"
#include "command_type.h"
#include "io_status.h"
#include <cstdio>
#include <cstring>

io_status start_db (list2<record> *db, int *res)
{
	io_status ret = io_status::success;
	char buf[LEN] = {};
	command x;
	FILE *f_out = stdout,
		 *f_in = stdin;

	(*res) = 0;

	while (fgets(buf, LEN, f_in))
	{
		char *saveptr = nullptr,
			 *cmd = buf;
		int is_break = 0;

		while ((cmd = strtok_r(cmd, ";\n", &saveptr)) != nullptr)
		{
			// Parse command
			if ((ret = x.parse(buf)) == io_status::format)
				fprintf(f_out, "Wrong format of request!\n\n");
			else if (ret != io_status::success)
			{
				is_break = 1;
				break;
			} else if (x.get_type() == command_type::quit)
			{
				is_break = 1;
				break;
			} else
				(*res) += x.apply(db);

			fprintf(f_out, "\n");
			cmd = nullptr;
		}

		if (is_break)
			break;
	}

	if (feof(f_in) != 0)
		ret = io_status::success;

	return ret;
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

