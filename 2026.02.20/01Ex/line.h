#ifndef LINE_H
#define LINE_H

#include <memory>
#include <cstring>

class line
{
	public:
		using uptr = std::unique_ptr<char[]>;

	private:
		uptr value;
	
	public:
		char * get () { return value.get(); }

		void read (const char *str)
		{
			const std::size_t n = std::strlen(str);
			value = std::make_unique<char[]>(n + 1);
			std::memcpy(value.get(), str, n + 1);
		}

		void print (FILE *fp = stdout) { fprintf(fp, "%s\n", value.get()); }

		int cmp (const line& x) const
		{
			if (value == nullptr)
			{
				if (x.value != nullptr)
					return -1;
				return 0;
			}

			if (x.value == nullptr)
				return 1;

			return strcmp(value.get(), x.value.get());
		}

		int cmp (const char *x) const
		{
			if (value == nullptr)
			{
				if (x != nullptr)
					return -1;
				return 0;
			}

			if (x == nullptr)
				return 1;

			return strcmp(value.get(), x);
		}

		int operator< (const line& x) const { return cmp(x) < 0; }
		int operator<= (const line& x) const { return cmp(x) <= 0; }
		int operator> (const line& x) const { return cmp(x) > 0; }
		int operator>= (const line& x) const { return cmp(x) >= 0; }
		int operator== (const line& x) const { return cmp(x) == 0; }
		int operator!= (const line& x) const { return cmp(x) != 0; }
};

#endif // LINE_H
