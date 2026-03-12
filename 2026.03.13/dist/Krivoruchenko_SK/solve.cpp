#include "solve.h"
#include "command_type.h"
#include "io_status.h"
#include <cstdio>

io_status start_db (list2<record> *db, int *res)
{
	io_status ret;
	int len = 0,
		symb = 0;
	char buf[LEN] = {};
	command x;
	FILE *f_out = stdout,
		 *f_in = stdin;

	(*res) = 0;

	while (((symb = fgetc(f_in)) != EOF) && (len < LEN))
	{
		// End of command
		if (symb == ';')
		{
			// Set end of the string
			buf[len] = '\0';

			// Parse command
			if ((ret = x.parse(buf)) == io_status::format)
				fprintf(f_out, "Wrong format of request!\n");
			else if (ret != io_status::success)
				break;
			else if (x.get_type() == command_type::quit)
				break;
			else
				(*res) += x.apply(db);

			fprintf(f_out, "\n");
			len = 0;
		} else if (symb != '\n' && symb != '\0')
		{
			// Write string
			buf[len] = (char)symb;
			len++;
		} else {
			buf[len] = ' ';
			len++;
		}
	}

	if (symb == EOF)
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

