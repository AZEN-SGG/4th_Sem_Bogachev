#include "solve.h"

io_status write_sentence_with_right_out (FILE *in, FILE *out, pattern *mister_x, const char *t, int *res)
{
	char word[LEN] = {},
		 buffer[LEN] = {};

	*res = 0;

	while (fgets(buffer, LEN, in) != nullptr)
	{
		int len = 0;

		// Идём по строке
		for (len = 0 ; buffer[len] != '\0' ; len++)
			word[len] = buffer[len];

		if (word[len - 1] == '\n')
			word[len - 1] = '\0';

		char *saveptr = nullptr,
			 *token = word;

		while ((token = strtok_r(token, t, &saveptr)))
		{
			if (mister_x->with_out(token, 0, 0))
			{
				fprintf(out, "%s", buffer);
				(*res)++;
				break;
			}

			token = nullptr;
		}
	}

	if (feof(in) == 0)
		return io_status::read;

	return io_status::success;
}

io_status solve_09 (char *f_in, char *f_out, char *s, const char *t, int *r)
{
	auto mister_x = std::make_unique<pattern>();
    if (mister_x == nullptr)
		return io_status::memory;

	io_status ret = mister_x->read(s);
	if (ret != io_status::success)
		return ret;

	FILE *in, *out;

	if ((in = fopen(f_in, "r")) == nullptr)
		return io_status::open;

	if ((out = fopen(f_out, "w")) == nullptr)
	{
		fclose(in);
		return io_status::open;
	}

	ret = write_sentence_with_right_out(in, out, mister_x.get(), t, r);
	
	fclose(in);
	fclose(out);

	return ret;
}

