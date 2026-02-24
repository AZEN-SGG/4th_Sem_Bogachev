#ifndef OPERATION_H
#define OPERATION_H

#include "io_status.h"

#include <memory>
#include <cstring>
#include <cstdio>

#define LEN 1234

class operation
{
	public:
		using uptr = std::unique_ptr<char[]>;
		using func = int(*)(const operation&, const char *);

	private:
		uptr key;
		func cmp_fn = nullptr;
	
	public:
		char * get_key () { return key.get(); }
		func get_fn () { return cmp_fn; }

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
						cmp_fn = &operation::lt;
					else if (v[0] == '<')
						cmp_fn = &operation::gt;
					else
						return io_status::format;
				} else if (v[1] == '=')
				{
					switch (v[0])
					{
						case '>':
							cmp_fn = &operation::le;
							break;
						case '<':
							cmp_fn = &operation::ge;
							break;
						case '=':
							cmp_fn = &operation::eq;
							break;
						default:			
							return io_status::format;
					}
				} else if (v[0] == '<' && v[1] == '>')
					cmp_fn = &operation::ne;
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

		static int lt (const operation& x, const char *str) { return x.cmp(str) < 0; }
		static int le (const operation& x, const char *str) { return x.cmp(str) <= 0; }
		static int gt (const operation& x, const char *str) { return x.cmp(str) > 0; }
		static int ge (const operation& x, const char *str) { return x.cmp(str) >= 0; }
		static int eq (const operation& x, const char *str) { return x.cmp(str) == 0; }
		static int ne (const operation& x, const char *str) { return x.cmp(str) != 0; }

		int is_valid (const char * x) const
		{
			if (cmp_fn != nullptr)
				return cmp_fn(*this, x);
			return 0;
		}
};

#endif // OPERATION_H
