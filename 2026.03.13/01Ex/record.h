#ifndef RECORD_H
#define RECORD_H

#include "io_status.h"
#include "condition.h"
#include "ordering.h"
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
				word = nullptr;

			return 0;
		}

		record (record&& x) noexcept : name_t(std::move(x))
		{
			phone = x.phone;
			group = x.group;

			x.phone = 0;
			x.group = 0;
		}
		record (const record& x) = delete;

		record& operator= (record&& x) noexcept
		{
			if (this != &x)
			{
				name_t::operator=(std::move(x));
				phone = x.phone;
				group = x.group;
				x.phone = 0;
				x.group = 0;
			}
			return *this;
		}
		record& operator= (const record& x) = delete;

		static int cmp_word (const record& x, const record& y)
		{
			int cmp = 0;

			if (x.word == nullptr)
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
					cmp = std::strcmp(x.word.get(), y.word.get());
			}

			return cmp;
		}
		static int cmp_phone (const record& x, const record& y) { return x.phone - y.phone; }
		static int cmp_group (const record& x, const record& y) { return x.group - y.group; }

		bool compare_word (condition x, const record& y) const { return compare(x, cmp_word(*this, y)); };
		bool compare_phone (condition x, const record& y) const { return compare(x, cmp_phone(*this, y)); }
		bool compare_group (condition x, const record& y) const { return compare(x, cmp_group(*this, y)); }

		void print (FILE *fp = stdout, const ordering *order = nullptr) const
		{
			const ordering default_ordering[ORDERING_LEN] = {ordering::name, ordering::phone, ordering::group};
			const ordering *p = (order ? order : default_ordering);

			for (int i = 0 ; i < ORDERING_LEN ; ++i)
			{
				switch (p[i])
				{
					case ordering::name:
						fprintf(fp, " %s", word.get());
						break;
					case ordering::phone:
						fprintf(fp, " %d", phone);
						break;
					case ordering::group:
						fprintf(fp, " %d", group);
						break;
					case ordering::none:
						continue;
				}
			}
			
			fprintf(fp, "\n");
		}
		
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
	protected:
		void erase ()
		{
			name_t::erase();
			phone = group = 0;
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
				case condition::nlike:
					return false;
			}

			return false;
		}
};

#endif // RECORD_H

