#include "solve.h"

int read_print (FILE *in, FILE *out, lines_t& lines)
{
	int num, res = 0;
	std::string line;
	char BUF[LEN] = {};
	
	while (fgets(BUF, LEN, in))
	{
		line = BUF;
		while (line.size() && line[line.size() - 1] == '\n')
		{
			BUF[line.size() - 1] = '\0';
			line.pop_back();
		}

		if (!line.empty())
		{
			auto it = lines.find(line);
			if (it == lines.end())
			{
				num = lines[std::move(line)] = 1;
				res++;
			} else
				num = ++lines[std::move(line)];

			fprintf(out, "%lu %d %s\n", lines.size(), num, BUF);
		}
	}

	return res;
}

io_status t4_solve (const char *f_in, const char *f_out, int& r)
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

