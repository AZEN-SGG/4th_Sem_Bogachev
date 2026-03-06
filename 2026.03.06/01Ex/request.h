#ifndef REQUEST_H
#define REQUEST_H

#include "io_status.h"
#include "operation.h"
#include "ordering.h"
#include "command.h"

#include <cstring>
#include <utility>
#include <memory>

#define MAX_LEN_REQUESTS 3

template <typename T>
class list2;

class request
{
	private:
		std::unique_ptr<command[]>	commands	= nullptr;
		std::unique_ptr<ordering[]>	order		= nullptr;
		int len_c	= 0;
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

		io_status parse_operations (char *cmd)
		{
			char *land = nullptr,
				 *lor = nullptr;
			operation link = operation::none;
			command oper[MAX_LEN_REQUESTS] = {};
			
			land = std::strstr(cmd, " and ");
			lor = std::strstr(cmd, " or ");

			len_c = 0;
			while ((land || lor) && (len_c < MAX_LEN_REQUESTS))
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

			if ((!(land || lor)) && (len_c < MAX_LEN_REQUESTS))
			{
				if (!oper[len_c].parse(cmd))
					return io_status::format;
				len_c++;
			}

			commands = std::make_unique<command[]>(len_c);
			if (!commands)
				return io_status::memory;

			for (int i = 0 ; i < len_c ; ++i)
				commands[i] = std::move(oper[i]);
			
			return io_status::success;
		}

		io_status parse_order (char *cmd)
		{
			char *saveptr = nullptr;
			order.reset();

			if (cmd[0] == '*' && cmd[1] == '\0')
				return io_status::success;

			order = std::make_unique<ordering[]>(ORDERING_LEN);
			if (!order)
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

			return io_status::success;
		}

		io_status parse (char *cmd)
		{
			io_status ret;
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

		friend class list2<request>;
	private:
		void erase ()
		{
			commands.reset();
			order.reset();
			len_c = 0;
		}
};

#endif // REQUEST_H
