#include "solve.h"
#include <algorithm>
#include <utility>

io_status read (FILE *in, dvec_t& lines)
{
	char BUF[LEN] = {};
	
	for (int num = 1 ; fgets(BUF, LEN, in) ; ++num)
		lines.emplace_back(BUF, std::make_pair(num, 0));

	return io_status::success;
}

int print (FILE *out, const dvec_t& lines) noexcept
{
	int res = 0;

	for (const dupl_t& el : lines)
	{
		res++;
		fprintf(out, "%d %d %s", el.second.first, el.second.second, el.first.c_str());
	}

	return res;
}

int number_duplicates (dvec_t& lines)
{
	if (lines.empty())
		return 0;

	auto sort_by_line = [](const dupl_t& x, const dupl_t& y)
	{
		int cmp = x.first.compare(y.first);

		if (cmp < 0)
			return true;

		if (cmp > 0)
			return false;

		// Отличается от первого, т.к. обход прямой
		return x.second < y.second;
	};

	std::sort(lines.begin(), lines.end(), sort_by_line);

	int num_diff = 1,
		len_dupl = 1;
	auto begin_section = lines.begin();
	// В начале проверили, что вектор не пуст
	for (auto it = lines.begin() + 1 ; it != lines.end() ; ++it)
	{
		if (begin_section->first == it->first)
			len_dupl++;
		else
		{
			num_diff++;
			for (auto jt = begin_section ; jt != it + 1 ; ++jt)
				jt->second.second = len_dupl;
			begin_section = it;
			len_dupl = 1;
		}
	}

	for (auto it = begin_section ; it != lines.end() ; ++it)
		it->second.second = len_dupl;
		
	return num_diff;
}

io_status t3_solve (const char *f_in, const char *f_out, int& r)
{
	io_status ret;
	dvec_t lines;

	ret = read_file(f_in, lines);
	if (ret != io_status::success)
		return ret;

	r = number_duplicates(lines);

	// Сортируем по второму полю
	std::sort(lines.begin(), lines.end(), [](const dupl_t& x, const dupl_t& y) { return x.second.first < y.second.first; });

	int temp = 0;
	ret = print_file(f_out, lines, temp);
	if (ret != io_status::success)
		return ret;
	
	return io_status::success;
}
