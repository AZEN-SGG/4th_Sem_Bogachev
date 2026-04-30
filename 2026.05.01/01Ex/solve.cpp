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

int print (FILE *out, const vec_t& vec) noexcept
{
	int res = 0;

	for (const vec_el_t& el : vec)
	{
		res++;
		fprintf(out, "%s", el.first.c_str());
	}

	return res;
}

int delete_duplicates (vec_t& vec)
{
	auto sort_by_line = [](const vec_el_t& x, const vec_el_t& y)
	{
		if (x.first < y.first)
			return true;

		if (x.first > y.first)
			return false;

		// Поздние первыми, так как мы итерируем с конца
		return x.second > y.second;
	};

	std::sort(vec.begin(), vec.end(), sort_by_line);

	int len_del = 0;
	vec_el_t *temp_ptr = nullptr,
			 temp;
	for (int i = vec.size() - 1 ; i >= 0 ; i--)
	{
		if (temp_ptr && ((*temp_ptr).first == vec[i].first))
		{
			temp = std::move(vec[vec.size() - 1 - len_del]);
			vec[vec.size() - 1 - len_del] = std::move(vec[i]);
			vec[i] = std::move(temp);
			len_del++;
		} else
			temp_ptr = &vec[i];
	}

	vec.erase(vec.end() - len_del, vec.end());

	return len_del;
}

io_status t1_solve (const char *f_in, const char *f_out, int& r)
{
	io_status ret;
	vec_t arr;

	ret = read_file(f_in, arr);
	if (ret != io_status::success)
		return ret;

	delete_duplicates(arr);

	// Сортируем по второму полю
	std::sort(arr.begin(), arr.end(), [](const vec_el_t& x, const vec_el_t& y) { return x.second < y.second; });

	ret = print_file(f_out, arr, r);
	if (ret != io_status::success)
		return ret;
	
	return io_status::success;
}
