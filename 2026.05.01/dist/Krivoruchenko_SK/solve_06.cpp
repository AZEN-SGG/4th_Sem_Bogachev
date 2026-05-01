#include "solve_06.h"
#include <algorithm>

io_status read (FILE *in, lwords_t& words)
{
	std::string buf;
	char BUF[LEN] = {};
	
	for (int num = 1 ; fgets(BUF, LEN, in) ; ++num)
	{
		buf = BUF;
		if ((!buf.empty()) && buf.back() == '\n')
			buf.pop_back();
		words.emplace_back(std::move(buf));
	}

	return io_status::success;
}

int number_contains (FILE *in, FILE *out, lwords_t& words, const char *t)
{
	words.sort();
	words.unique();

	int len_common = 0;
	std::string temp;
	char BUF[LEN] = {},
		 *saveptr = nullptr;

	while (fgets(BUF, LEN, in))
	{
		int len_contains = 0,
			len = 0;

		temp = BUF;
		len = temp.length();

		if (BUF[len - 1] == '\n')
			temp[len - 1] = BUF[len - 1] = '\0';

		for (char *str = BUF ; (str = strtok_r(str, t, &saveptr)) ; str = nullptr)
			len_contains += (std::find(words.begin(), words.end(), str) != words.end());
		
		fprintf(out, "%d %s\n", len_contains, temp.c_str());
		len_common += len_contains;
	}

	return len_common;
}

io_status t6_solve (const char *a, const char *b, const char *f_out, const char *t, int& r)
{
	io_status ret;
	lwords_t words;

	ret = read_file(a, words);
	if (ret != io_status::success)
		return ret;

	FILE *in = fopen(b, "r");
	if (!in)
		return io_status::open;

	FILE *out = fopen(f_out, "w");
	if (!out)
		return io_status::open;

	r = number_contains(in, out, words, t);

	fclose(in);
	fclose(out);
	return io_status::success;
}
