#ifndef DATABASE_H
#define DATABASE_H

#include "io_status.h"
#include "list2.h"
#include "list2_node.h"
#include "ordering.h"
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
	std::unique_ptr<name_index_t> name_index = nullptr;
	std::unique_ptr<phone_index_t> phone_index = nullptr;

public:
	database ()
	{
		db = std::make_unique<list2<record>>();
		name_index = std::make_unique<name_index_t>();
		phone_index = std::make_unique<phone_index_t>();
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

			auto name_node = name_index->add(curr);
			if (!name_node)
				return io_status::memory;
			name_index->fix_tree(name_node);

			auto phone_node = phone_index->add(curr);
			if (!phone_node)
				return io_status::memory;
			phone_index->fix_tree(phone_node);

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
//					x.print();
					(*res) += x.apply(db.get(), name_index.get(), phone_index.get());
//					name_index->print(100);
//					phone_index->print(100);
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
	void erase () { db->erase(); name_index->erase(); phone_index->erase(); }
};

#endif // DATABASE_H
