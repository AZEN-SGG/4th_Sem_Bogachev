#ifndef RECORD_H
#define RECORD_H

#include "io_status.h"
#include "condition.h"
#include "pattern.h"

#include <memory>
#include <cstdio>
#include <cstring>

#define LEN 1234

class record : virtual public name_t
{
	protected:
		int phone = 0;
		int group = 0;
	public:
		record () = default;
		~record () = default;

		const char * get_word () const { return word.get(); }
		int get_phone () const { return phone; }
		int get_group () const { return group; }

		int init (const char *n, int p, int g)
		{
			phone = p;
			group = g;

			if (n)
			{
				word = std::make_unique<char []> (std::strlen(n) + 1);
				if (!word)
					return -1;
				std::strcpy(word.get(), n);
			} else
				n = nullptr;

			return 0;
		}

		record (record&& x) = default;
		record (const record& x) = delete;

		record& operator= (record&& x) = default;
		record& operator= (const record& x) = delete;

		bool compare_word (condition x, const record& y) const
		{
			int cmp = 0;
			
			if (word == nullptr)
			{
				if (y.word == nullptr)
					cmp = 0;
				else
					cmp = -1;
			} else
			{
				if (y.word == nullptr)
					cmp = 1;
				else
					cmp = std::strcmp(word.get(), y.word.get());
			}

			return compare(x, cmp);
		};

		bool compare_phone (condition x, const record& y) const { return compare(x, phone - y.phone); }
		bool compare_group (condition x, const record& y) const { return compare(x, group - y.group); }

		void print (FILE *fp = stdout) const { fprintf (fp, "%s %d %d\n\n", word.get(), phone, group); }
		
		io_status read (FILE *fp = stdin)
		{
			char buf[LEN];
			word = nullptr;

			if (fscanf(fp, "%s%d%d", buf, &phone, &group) != 3)
			{
				if (feof(fp))
					return io_status::eof;
				return io_status::format;
			}

			if (init (buf, phone, group))
				return io_status::memory;

			return io_status::success;
		}
	private:
		static bool compare (condition x, int cmp)
		{
			switch (x)
			{
				case condition::none:
					return true;
				case condition::eq:
					return cmp == 0;
				case condition::ne:
					return cmp != 0;
				case condition::lt:
					return cmp < 0;
				case condition::gt:
					return cmp > 0;
				case condition::le:
					return cmp <= 0;
				case condition::ge:
					return cmp >= 0;
				case condition::like:
					return false;
			}

			return false;
		}
};

#endif // RECORD_H

