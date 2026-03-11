#include "solve.h"
#include "io_status.h"
#include "request.h"


io_status start_db (const list2<record> *db, int *res)
{
	io_status ret;
	int len = 0,
		symb = 0;
	char buf[LEN] = {},
	command x;
	FILE *f_out = stdout,
		 *f_in = stdin;

	(*res) = 0;

	while (((symb = fgetc(f_in)) != EOF) && (len < LEN))
	{
		if (symb == ';')
		{
			if (len == 0)
				break;
			buf[len] = '\0';
			if ((ret = x.parse(buf)) == io_status::format)
				fprintf(f_out, "Wrong format of request!\n\n");
			else if (ret != io_status::success)
				return ret;
			else
				(*res) += db->print_valid(x, f_out);
			len = 0;
		} else if (symb != '\n' && symb != '\0')
		{
			buf[len] = (char)symb;
			len++;
		}
	}

	if (symb != EOF)
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

