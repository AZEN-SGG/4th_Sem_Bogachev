#include "solve_02.h"

int inclusion (char *str, const rb_tree<line> *olha, const char *delim)
{
	char *saveptr = nullptr;

	if ((str = strtok_r(str, delim, &saveptr)) == nullptr)
		return 0;

	do {
		if (olha->find(str) == nullptr)
			return 0;
	} while ((str = strtok_r(nullptr, delim, &saveptr)) != nullptr);

	return 1;
}

io_status find_inclusion (FILE *in, FILE *out, const rb_tree<line> *olha, const char *t, int *res)
{
	char buffer[LEN] = {},
		 words[LEN] = {};

	*res = 0;

	while (fgets(buffer, LEN, in) != nullptr)
	{
		int len = 0;
		for (len = 0 ; buffer[len] != '\0' ; len++)
			words[len] = buffer[len];
		
		if (words[len - 1] == '\n')
			words[len - 1] = '\0';

		if (inclusion(words, olha, t))
		{
			(*res)++;
			fprintf(out, "%s", buffer);
		}
	}

	if (feof(in) == 0)
		return io_status::read;

	return io_status::success;
}

io_status solve_02 (char *f_in, char *f_out, char *s, const char *t, int *r)
{
	std::unique_ptr<rb_tree<line>> olha = std::make_unique<rb_tree<line>>();
    if (olha == nullptr)
		return io_status::memory;

	io_status ret = olha->read(s, t);
	if (ret != io_status::success)
		return ret;

// Отладочная
	olha->print(100);

	FILE *in, *out;

	if ((in = fopen(f_in, "r")) == nullptr)
		return io_status::open;

	if ((out = fopen(f_out, "w")) == nullptr)
	{
		fclose(in);
		return io_status::open;
	}

	ret = find_inclusion(in, out, olha.get(), t, r);
	
	fclose(in);
	fclose(out);

	return io_status::success;
}
