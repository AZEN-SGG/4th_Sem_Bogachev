#ifndef STUDENT_H
#define STUDENT_H

#include "io_status.h"

#include <stdio.h>
#include <string.h>
#include <memory>

template <typename T>
class avl_tree;

class student
{
	private:
		std::unique_ptr<char []>	name;
		int	value = 0;

	public:
		student () = default;
		~student () = default;
		
		student (const student& x) = delete;
		student (student&& x) = default;
	
		student& operator= (const student& x) = delete;
		student& operator= (student&& x) = default;

		void print (FILE *fp = stdout) const
		{
			fprintf(fp, "%s %d\n", name.get(), value);
		}

		io_status read (FILE *fp = stdin)
		{
			const int LEN = 1234;
			char buf[LEN];
			int v;

			if (fscanf(fp, "%s%d", buf, &v) != 2)
				return io_status::format;

			erase();
			return init(buf, v);
		}

		int cmp (const student& x) const
		{
			if (name == nullptr)
			{
				if (x.name != nullptr)
					return -1;

				return value - x.value;
			}

			if (x.name == nullptr)
				return 1;

			int res = strcmp(name.get(), x.name.get());
			if (res)
				return res;

			return value - x.value;
		}

		int operator< (const student& x) const { return cmp(x) < 0; }
		int operator<= (const student& x) const { return cmp(x) <= 0; }
		int operator> (const student& x) const { return cmp(x) > 0; }
		int operator>= (const student& x) const { return cmp(x) >= 0; }
		int operator== (const student& x) const { return cmp(x) == 0; }
		int operator!= (const student& x) const { return cmp(x) != 0; }

		friend class avl_tree<student>;
	private:
		io_status init (const char *n, int v)
		{
			value = v;
			if (n != nullptr)
			{
				size_t len = strlen(n);
				name = std::make_unique<char[]>(len + 1);
				if (name != nullptr)
				{
					for (size_t i = 0; i <= len; ++i)
						name[i] = n[i];
				} else
					return io_status::memory;
			}

			return io_status::success;
		}

		void erase () { value = 0; name.reset(); }
};

#endif // STUDENT_H
