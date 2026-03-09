#ifndef REQUEST_H
#define REQUEST_H

#include "io_status.h"
#include "operation.h"
#include "ordering.h"
#include "command.h"
#include "command_type.h"

#include <cstring>
#include <utility>
#include <memory>


char * skip_spaces (char *str, const char *div)
{
	bool ch[256] = {};
	
	for (int i = 0 ; div[i] != '\0' ; ++i)
		ch[(unsigned int)div[i]] = 1;
	int i = 0;
	for (; str[i] || ch[(unsigned int)str[i]] ; ++i);

	return str + i;
}

template <typename T>
class list2;

class request
{
	private:
		static const int max_items = 3;
		std::unique_ptr<command[]>	commands	= nullptr;
		std::unique_ptr<ordering[]>	order		= nullptr;
		std::unique_ptr<ordering[]> order_by	= nullptr;
		command_type type	= command_type::none;
		int len_c			= 0;
	public:
		request()	= default;
		~request()	= default;

		io_status init (int len)
		{
			commands = std::make_unique<command[]>(len);
			if (!commands)
				return io_status::memory;

			order = std::make_unique<ordering[]>(ORDERING_LEN);
			if (!order)
				return io_status::memory;

			order_by = std::make_unique<ordering[]>(ORDERING_LEN);
			if (!order_by)
				return io_status::memory;

			len_c = len;
			return io_status::success;
		}

		void print (FILE *fp) const
		{
			if (order)
			{
				for (int i = 0 ; i < ORDERING_LEN ; ++i)
					switch (order[i])
					{
						case ordering::name:
							fprintf(fp, " name,");
							break;
						case ordering::phone:
							fprintf(fp, " phone,");
							break;
						case ordering::group:
							fprintf(fp, " group,");
							break;
						case ordering::none:
							fprintf(fp, " none,");
							break;
					}

				fprintf(fp, "\n");
			} else
				fprintf(fp, "nullptr\n");
			for (int i = 0 ; i < len_c ; ++i)
				commands[i].print(fp);
		}

		char * parse_command(char *cmd)
		{
			cmd = skip_spaces(cmd, " \t");
			
			switch (cmd[0])
			{
				case 'q':
					if ((cmd = strstr(cmd, "quit")) != nullptr)
					{
						cmd += 4;
						type = command_type::quit;
					}
					break;
				case 'i':
					if ((cmd = strstr(cmd, "insert")) != nullptr)
					{
						cmd += 6;
						type = command_type::insert;
					}
					break;
				case 's':
					if ((cmd = strstr(cmd, "select")) != nullptr)
					{
						cmd += 6;
						type = command_type::select;
					}
					break;
				case 'd':
					if ((cmd = strstr(cmd, "delete")) != nullptr)
					{
						cmd += 6;
						type = command_type::del;
					}
					break;
			}

			return cmd;
		}

		io_status parse_insert (char *cmd)
		{
			char buf[LEN] = {};
			if (sscanf(cmd, "(%s,%d,%d)", buf, &commands[0].phone, &commands[0].group) != 3)
				return io_status::format;

			commands[0].word = std::make_unique<char[]>(strlen(buf));

			int i = 0;
			for (; buf[i] != '\0' ; ++i)
				commands[0].word[i] = buf[i];
			commands[0].word[i] = '\0';

			return io_status::success;
		}

		io_status parse_select (char *cmd)
		{
			io_status ret;
			char *where = nullptr,
				 *in_order = nullptr;
			if ((where = std::strstr(cmd, " where ")) != nullptr)
			{
				where[0] = '\0';
				where += 7;

				if ((in_order = std::strstr(where, " order_by ")) != nullptr)
				{
					in_order[0] = '\0';
					in_order += 10;
					if ((ret = parse_order(in_order)) != io_status::success)
						return ret;
				}

				if ((ret = parse_operations(where)) != io_status::success)
					return ret;
			} else if ((in_order = std::strstr(cmd, " order_by ")) != nullptr)
			{
				in_order[0] = '\0';
				in_order += 10;
				if ((ret = parse_order(in_order)) != io_status::success)
					return ret;
			}

			if ((ret = parse_output(cmd)) != io_status::success)
				return ret;

			return io_status::success;
		}

		io_status parse_operations (char *cmd)
		{
			char *land = nullptr,
				 *lor = nullptr;
			operation link = operation::none;
//			command oper[max_items] = {};
			command *oper = commands.get();
			if (commands == nullptr)
				return io_status::memory;
// За счёт малого max_items можно один раз проинициализировать массив и далее его использовать

			land = std::strstr(cmd, " and ");
			lor = std::strstr(cmd, " or ");

			len_c = 0;
			while ((land || lor) && (len_c < max_items))
			{
				if (land)
				{
					if (lor)
					{
						if (land < lor)
						{
							link = operation::land;
							land[0] = '\0';
							land += 5;
						} else
						{
							link = operation::lor;
							lor[0] = '\0';
							lor += 4;
						}
					} else
					{
						link = operation::land;
						land[0] = '\0';
						land += 5;
					}
				} else
				{
					link = operation::lor;
					lor[0] = '\0';
					lor += 4;
				}

				if (!oper[len_c].parse(cmd))
					return io_status::format;

				switch (link)
				{
					case operation::land:
						cmd = land;
						land = strstr(cmd, " and ");
						break;
					case operation::lor:
						cmd = lor;
						lor = strstr(cmd, " or ");
						break;
					case operation::none:
						return io_status::format;
				}

				oper[len_c].op = link;
				len_c++;
			}

			if ((!(land || lor)) && (len_c < max_items))
			{
				if (!oper[len_c].parse(cmd))
					return io_status::format;
				len_c++;
			}

//			commands = std::make_unique<command[]>(len_c);
//			if (!commands)
//				return io_status::memory;
//
//			for (int i = 0 ; i < len_c ; ++i)
//				commands[i] = std::move(oper[i]);
			
			return io_status::success;
		}

		io_status parse_output (char *cmd)
		{
			char *saveptr = nullptr;
//			order.reset();

			if (cmd[0] == '*' && cmd[1] == '\0')
				return io_status::success;

//			order = std::make_unique<ordering[]>(ORDERING_LEN);
			if (order == nullptr)
				return io_status::memory;

			int len = 0;
			while ((cmd = strtok_r(cmd, ", ", &saveptr)) && (len < ORDERING_LEN))
			{
				switch (cmd[0])
				{
					case 'n':
						if (strncmp(cmd, "name", 5) == 0)
							order[len] = ordering::name;
						break;
					case 'p':
						if (strncmp(cmd, "phone", 6) == 0)
							order[len] = ordering::phone;
						break;
					case 'g':
						if (strncmp(cmd, "group", 6) == 0)
							order[len] = ordering::group;
						break;
					default:
						return io_status::format;
				}

				len++;
				cmd = nullptr;
			}

			for (int i = len ; i < ORDERING_LEN ; ++i)
				order[i] = ordering::none;

			return io_status::success;
		}

		io_status parse (char *cmd)
		{
			io_status ret;
			char *ops;
			cmd = parse_command(cmd);
			switch (type)
			{
				case command_type::insert:
					ret = parse_insert(cmd);
					break;
				case command_type::select:
					ret = parse_select(cmd);
					break;
				case command_type::del:
					ret = parse_delete(cmd);
					break;
				case command_type::quit:
					ret = io_status::success;
					break;
				case command_type::none:
					ret = io_status::format;
					break;
			}

			char *ops = std::strstr(cmd, " where ");
			if (ops == nullptr)
				return io_status::format;

			ops[0] = '\0';
			ops += 7; // add length of " where "

			ret = parse_operations(ops);
			if (ret != io_status::success)
				return ret;

			ret = parse_order(cmd);
			if (ret != io_status::success)
				return ret;

			return io_status::success;
		}

		bool apply (const record& x)
		{
			int flag = 0;
			operation op = operation::lor;

			for (int i = 0 ; i < len_c ; ++i)
			{
				bool res = commands[i].apply(x);
				if (op == operation::lor)
				{
					if (flag)
						return true;
					flag = res;
				} else
					flag = (res && flag);

				op = commands[i].op;
			}

			return flag;
		}

		friend class list2<record>;
	private:
		void erase ()
		{
			commands.reset();
			order.reset();
			len_c = 0;
		}
};

#endif // REQUEST_H
