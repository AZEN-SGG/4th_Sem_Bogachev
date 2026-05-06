#include "solve.h"
#include <cstdio>
#include <string>
#include <utility>

io_status read_print (FILE *in, FILE *out, lines_t& lines)
{
	std::string line;
	char BUF[LEN] = {};
	
	while (fgets(BUF, LEN, in))
	{
		line = BUF;
		while (line.size() && line[line.size() - 1] == '\n')
			line.pop_back();

		if (!line.empty())
		{
			auto res = lines.insert({std::move(line), 0});
			if (res.second)
				fprintf(out, "%s\n", res.first->first.c_str());
		}
	}

	return io_status::success;
}

io_status t2_solve (const char *f_in, const char *f_out, int& r)
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

	read_print(in, out, lines);
	r = lines.size();

	fclose(in);
	fclose(out);

	return io_status::success;
}

