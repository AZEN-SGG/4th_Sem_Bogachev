#include "solve.h"

io_status read (FILE *in, list_t& lines)
{
	char BUF[LEN] = {};
	
	for (int num = 1 ; fgets(BUF, LEN, in) ; ++num)
		lines.push_back(std::make_pair(BUF, num));

	return io_status::success;
}

int print (FILE *out, const list_t& lines) noexcept
{
	int res = 0;

	for (const line_t& el : lines)
	{
		res++;
		fprintf(out, "%s", el.first.c_str());
	}

	return res;
}

int delete_duplicates (list_t& lines)
{
	auto sort_by_line = [](const line_t& x, const line_t& y)
	{
		int cmp = x.first.compare(y.first);

		if (cmp < 0)
			return true;

		if (cmp > 0)
			return false;

		// Отличается от первого, т.к. обход прямой
		return x.second < y.second;
	};

	lines.sort(sort_by_line);

	int len_del = 0;
	line_t *temp_ptr = nullptr;
	for (auto it = lines.begin() ; it != lines.end() ;)
	{
		if (temp_ptr && (temp_ptr->first == it->first))
		{
			it = lines.erase(it);
			len_del++;
		} else
		{
			temp_ptr = &(*it);
			it++;
		}
	}

	return len_del;
}

io_status t2_solve (const char *f_in, const char *f_out, int& r)
{
	io_status ret;
	list_t lines;

	ret = read_file(f_in, lines);
	if (ret != io_status::success)
		return ret;

	delete_duplicates(lines);

	// Сортируем по второму полю
	lines.sort([](const line_t& x, const line_t& y) { return x.second < y.second; });

	ret = print_file(f_out, lines, r);
	if (ret != io_status::success)
		return ret;
	
	return io_status::success;
}
