#ifndef REQUEST_H
#define REQUEST_H

#include "io_status.h"
#include "operation.h"
#include "ordering.h"
#include "command.h"

#include <cstring>
#include <utility>
#include <memory>



template <typename T>
class list2;

class request
{
	private:
		static const int max_items = 3;
		std::unique_ptr<command[]>	commands	= nullptr;
		std::unique_ptr<ordering[]>	order		= nullptr;
		std::unique_ptr<ordering[]> order_by	= nullptr;
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
