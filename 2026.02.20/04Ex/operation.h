#ifndef OPERATION_H
#define OPERATION_H

#include "io_status.h"

#include <memory>
#include <cstring>

#define LEN 1234

class operation
{
	public:
		using uptr = std::unique_ptr<char[]>;
		using cmpFn = int(*)(const operation&, const char *);

	private:
		uptr key;
		cmpFn func;
	
	public:
		char * get_key () { return key.get(); }
		cmpFn get_fn () { return func; }

		io_status read (const char *k, const char *v)
		{
			std::size_t n;
			if (k)
			{
				n = std::strlen(k);
				key = std::make_unique<char[]>(n + 1);
				std::memcpy(key.get(), k, n + 1);
			} else
				return io_status::format;

			if (v)
			{
				if (v[1] == '\0')
				{
					if (v[0] == '>')
						func = &operation::lt;
					else if (v[0] == '<')
						func = &operation::gt;
				} else if (v[1] == '=')
				{
					switch (v[0])
					{
						case '>':
							func = &operation::le;
							break;
						case '<':
							func = &operation::ge;
							break;
						case '=':
							func = &operation::eq;
							break;
					}
				} else if (v[0] == '<' && v[1] == '>')
					func = &operation::ne;
				else
					return io_status::format;
			} else
				return io_status::format;

			return io_status::success;
		}

		void print (FILE *fp = stdout) const { fprintf(fp, "%s\n", key.get()); }

		int cmp (const operation& x) const
		{
			if (key == nullptr)
			{
				if (x.key != nullptr)
					return -1;
				return 0;
			}

			if (x.key == nullptr)
				return 1;

			return strcmp(key.get(), x.key.get());
		}

		int cmp (const char *x) const
		{
			if (key == nullptr)
			{
				if (x != nullptr)
					return -1;
				return 0;
			}

			if (x == nullptr)
				return 1;

			return strcmp(key.get(), x);
		}

		static int lt (const operation& x, const char *str) { x.cmp(str); }
		static int le (const operation& x, const char *str) { x.cmp(str); }
		static int gt (const operation& x, const char *str) { x.cmp(str); }
		static int ge (const operation& x, const char *str) { x.cmp(str); }
		static int eq (const operation& x, const char *str) { x.cmp(str); }
		static int ne (const operation& x, const char *str) { x.cmp(str); }
};

#endif // OPERATION_H
