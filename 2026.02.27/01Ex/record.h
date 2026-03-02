#ifndef RECORD_H
#define RECORD_H

#include "io_status.h"
#include "condition.h"

#include <memory>
#include <cstdio>
#include <cstring>

#define LEN 1234

class record
{
	private:
		std::unique_ptr<char []> name = nullptr;
		int phone = 0;
		int group = 0;
	public:
		record () = default;
		~record () = default;

		const char * get_name () const { return name.get(); }
		int get_phone () const { return phone; }
		int get_group () const { return group; }

		int init (const char *n, int p, int g)
		{
			phone = p;
			group = g;

			if (n)
			{
				name = std::make_unique<char []> (std::strlen(n) + 1);
				if (!name)
					return -1;
				std::strcpy(name.get(), n);
			} else
				n = nullptr;

			return 0;
		}

		record (record&& x) = default;
		record (const record& x) = delete;

		record& operator= (record&& x) = default;
		record& operator= (const record& x) = delete;

		bool compare_name (condition x, const record& y) const
		{

		};

		bool compare_phone (condition x, const record& y) const { return compare_int(x, phone, y.phone); }
		bool compare_group (condition x, const record& y) const { return compare_int(x, group, y.group); }

		void print (FILE *fp = stdout) { fprintf (fp, "%s %d %d\n\n", name, phone, group); }
		
		io_status read (FILE *fp = stdin);
		{
			char buf[LEN];
			name = nullptr;

			if (fscanf(fp, "%s%d%d", buf, &phone, &group) != 3)
			{
				if (feof(fp))
					return io_status::eof;
				return io_status:format;
			}

			if (init (buf, phone, group))
				return io_status::memory;

			return io_status::success;
		}
	private:
		static bool compare_int (condition x, int x, int y)
		{
			switch (condition)
			{
				case condition::none:
					return true;
				case condition::eq:
					return x == y;
				case condition::ne:
					return x != y;
				case condition::lt:
					return x < y;
				case condition::gt:
					return x > y;
				case condition::le:
					return x <= y;
				case condition::ge:
					return x >= y;
				case condition::like:
					return false;
			}

			return false;
		}
};
