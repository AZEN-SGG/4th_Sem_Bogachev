#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include "database.h"

#include "io_status.h"
#include "record.h"
#include "command.h"

#include <memory>

class database_manager
{
private:
	std::unique_ptr<database<record>> db = nullptr;
public:
	database_manager() { db = std::make_unique<database<record>>(); }

	io_status read_file (char *filename, const unsigned int max_read = -1)
	{
		FILE *fp = fopen(filename, "r");
		if (fp == nullptr)
			return io_status::open;

		io_status ret = db->read(fp, max_read);

		fclose(fp);
		return ret;
	}

	io_status start (int *res)
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
				{
//					x.print();
					ret = x.apply(db.get(), res);
//					db->print(100);
					if (ret != io_status::success)
						return ret;
				}

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
};

#endif // DATABASE_MANAGER_H
