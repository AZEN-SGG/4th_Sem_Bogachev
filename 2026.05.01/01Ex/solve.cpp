#include "solve.h"
#include "io_status.h"
#include <algorithm>
#include <cstdio>
#include <utility>

io_status read (FILE *in, vec_t& vec)
{
	char BUF[LEN] = {};
	
	for (int num = 1 ; fgets(BUF, LEN, in) ; ++num)
		vec.push_back(std::make_pair(BUF, num));

	return io_status::success;
}

int print (FILE *out, vec_t& vec) noexcept
{
	int res = 0;

	for (const vec_el_t& el : vec)
	{
		res++;
		fprintf(out, "%s", el.first.c_str());
	}

	return res;
}

io_status t1_solve (const char *f_in, const char *f_out, int& r)
{
	io_status ret;
	vec_t arr;

	ret = read_file(f_in, arr);
	if (ret != io_status::success)
		return ret;

	auto sort_by_line = [](const vec_el_t& x, const vec_el_t& y)
	{
		if (x.first < y.first)
			return true;

		if (x.first > y.first)
			return false;

		return x.second < y.second;
	};

	std::sort(arr.begin(), arr.end(), sort_by_line);

	ret = print_file(f_out, arr, r);
	if (ret != io_status::success)
		return ret;
	
	return io_status::success;
}
