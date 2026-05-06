#include "solve.h"
#include <iterator>

int read_print (FILE *in, FILE *out, lines_t& lines)
{
	int res = 0;
	std::string line;
	char BUF[LEN] = {};
	
	while (fgets(BUF, LEN, in))
	{
		line = BUF;
		while (line.size() && line[line.size() - 1] == '\n')
			line.pop_back();

		if (!line.empty())
		{
			auto it = lines.insert(std::move(line));
			auto range = lines.equal_range(*it);
			auto dis = std::distance(range.first, range.second);
			res += dis == 1;
			fprintf(out, "%lu %ld %s\n", lines.size(), dis, it->c_str());
		}
	}

	return res;
}

io_status t3_solve (const char *f_in, const char *f_out, int& r)
{
	lines_t lines;

	FILE *in = fopen(f_in, "r");
	if (!in)
		return io_status::open;

	FILE *out = fopen(f_out, "w");
	if (!out)
	{
		fclose(in);
		return io_status::open;
	}

	r = read_print(in, out, lines);

	fclose(in);
	fclose(out);

	return io_status::success;
}

