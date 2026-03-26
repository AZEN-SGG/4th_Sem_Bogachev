#ifndef DATABASE_H
#define DATABASE_H

#include "io_status.h"
#include "list2.h"
#include "list2_node.h"
#include "record.h"
#include "command.h"
#include "rb_tree.h"
#include "data_tree.h"

#include <memory>
#include <new>

class database
{
private:
	std::unique_ptr<list2<record>> db = nullptr;
	std::unique_ptr<rb_tree<data_tree<record, char *>>> fast_db = nullptr;

public:
	database ()
	{
		db = std::make_unique<list2<record>>();
		fast_db = std::make_unique<rb_tree<data_tree<record, char *>>>();
	}
	~database () = default;

	io_status read (FILE *fp, const unsigned int max_read = -1)
	{
		list2_node<record> buf = {},
			*curr = nullptr;

		unsigned int i = 0;
		while ((i < max_read) && (buf.read(fp) == io_status::success))
		{
			curr = new (std::nothrow) list2_node<record>;

			if (!curr)
			{
				erase();
				return io_status::memory;
			}

			*curr = (list2_node<record> &&)buf;
			db->add(curr);

			auto new_node = fast_db->add<list2_node<record>>(curr);
			if (!new_node)
				return io_status::memory;

			fast_db->fix_tree(new_node);
			i++;
		} if ((!feof(fp)) && (i < max_read))
		{
			erase();
			return io_status::format;
		}

		return io_status::success;
	}

	io_status read_file (char *filename, const unsigned int max_read = -1)
	{
		FILE *fp = fopen(filename, "r");
		if (fp == nullptr)
			return io_status::open;

		io_status ret = read(fp, max_read);

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
	//				x.print();
					(*res) += x.apply(db.get(), fast_db.get());
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
private:
	void erase () { db->erase(); fast_db->erase(); }
};

#endif // DATABASE_H
