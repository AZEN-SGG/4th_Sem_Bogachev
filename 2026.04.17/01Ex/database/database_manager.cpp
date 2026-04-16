#include "database_manager.h"
#include "io_status.h"


io_status database_manager::read_file (char *filename, const unsigned int max_read)
{
	FILE *fp = fopen(filename, "r");
	if (fp == nullptr)
		return io_status::open;

	io_status ret = db->read(fp, max_read);

	fclose(fp);
	return ret;
}

io_status database_manager::query_handler (char *buf, int *res, FILE *f_out)
{
	io_status ret = io_status::success;
	command x;

	(*res) = 0;
	char *saveptr = nullptr,
		 *cmd = buf;

	while ((cmd = strtok_r(cmd, ";\n", &saveptr)) != nullptr)
	{
		// Parse command
		if ((ret = x.parse(cmd)) == io_status::format)
			fprintf(f_out, "Wrong format of request!\n\n");
		else if (ret != io_status::success)
			break;
		else if (x.get_type() == command_type::quit)
		{
			ret = io_status::quit;
			break;
		} else
		{
			ret = x.apply(db.get(), res, f_out);
			if (ret != io_status::success)
				break;
		}

		fprintf(f_out, "\n");
		cmd = nullptr;
	}

	return ret;
}
