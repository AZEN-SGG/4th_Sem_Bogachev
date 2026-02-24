#include "solve.h"


io_status write_good_sentence (FILE *in, FILE *out, const list<operation> *shai_hulud, const char *t, int *res)
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
			token = nullptr;
			
			if (shai_hulud->fit_one(token))
			{
				(*res)++;
				fprintf(out, "%s", buffer);
				break;
			}

			token = nullptr;
		}
	}

	if (feof(in) == 0)
		return io_status::read;

	return io_status::success;
}

io_status solve_04 (char *f_in, char *f_out, char *s, char *x, const char *t, int *r)
{
	auto shai_hulud = std::make_unique<list<operation>>();
    if (shai_hulud == nullptr)
		return io_status::memory;

	io_status ret = shai_hulud->read(s, x, t);
	if (ret != io_status::success)
		return ret;

// Отладочная
	shai_hulud->print(100);

	FILE *in, *out;

	if ((in = fopen(f_in, "r")) == nullptr)
		return io_status::open;

	if ((out = fopen(f_out, "w")) == nullptr)
	{
		fclose(in);
		return io_status::open;
	}

	ret = write_good_sentence(in, out, shai_hulud.get(), t, r);
	
	fclose(in);
	fclose(out);

	return ret;
}
