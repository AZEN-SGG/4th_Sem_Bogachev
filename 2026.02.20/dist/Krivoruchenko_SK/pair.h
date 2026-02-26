#ifndef PAIR_H
#define PAIR_H

#include <memory>
#include <cstring>

#define LEN 1234

class pair
{
	public:
		using uptr = std::unique_ptr<char[]>;

	private:
		uptr key;
		uptr value;
	
	public:
		char * get_key () { return key.get(); }
		char * get_value () { return value.get(); }

		void read (const char *k, const char *v)
		{
			std::size_t n;
			if (k)
			{
				n = std::strlen(k);
				key = std::make_unique<char[]>(n + 1);
				std::memcpy(key.get(), k, n + 1);
			} else
				key.reset();

			if (v)
			{
				n = std::strlen(v);
				value = std::make_unique<char[]>(n + 1);
				std::memcpy(value.get(), v, n + 1);
			} else
				value.reset();
		}

		void print (FILE *fp = stdout) const { fprintf(fp, "%s %s\n", key.get(), value.get()); }

		int cmp (const pair& x) const
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

		int operator< (const pair& x) const { return cmp(x) < 0; }
		int operator<= (const pair& x) const { return cmp(x) <= 0; }
		int operator> (const pair& x) const { return cmp(x) > 0; }
		int operator>= (const pair& x) const { return cmp(x) >= 0; }
		int operator== (const pair& x) const { return cmp(x) == 0; }
		int operator!= (const pair& x) const { return cmp(x) != 0; }
};

#endif // PAIR_H
