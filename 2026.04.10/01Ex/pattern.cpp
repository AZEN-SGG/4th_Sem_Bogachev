#include "pattern.h"

io_status pattern::read_word (char *str)
{
	io_status ret;
	int len = 0, b_len = 0;
	type_pattern p_buf[LEN] = {};
	borders b_buf[LEN] = {};

	ret = parsing_pattern(str, p_buf, &len, b_buf, &b_len);
	if (ret != io_status::success)
		return ret;

	word = std::make_unique<char[]>(len + 1);
	spec = std::make_unique<type_pattern[]>(len + 1);
	span = std::make_unique<borders[]>(b_len);

	for (int i = 0 ; str[i] != '\0' ; ++i)
	{
		word[i] = str[i];
		spec[i] = p_buf[i];
	}

	word[len] = '\0';

	for (int i = 0 ; i < b_len ; ++i)
		span[i] = b_buf[i];

	return io_status::success;
}

void pattern::print (FILE *fp) const
{
	if (word == nullptr)
	{
		fprintf(fp, "\nnullptr\nnullptr\n");
		return;
	} else
		fprintf(fp, "\n%s\n", word.get());

	int b_len = 0;
	for (int len = 0 ; word[len] != '\0' ; len++)
	{
		switch (spec[len])
		{
			case type_pattern::none:
				fprintf(fp, "N");
				break;
			case type_pattern::one:
				fprintf(fp, "O");
				break;
			case type_pattern::many:
				fprintf(fp, "M");
				break;
			case type_pattern::in:
				fprintf(fp, "[%c-%c]", span[b_len].start, span[b_len].end);
				b_len++;
				break;
			case type_pattern::out:
				fprintf(fp, "[^%c-%c]", span[b_len].start, span[b_len].end);
				b_len++;
				break;
		}
	}

	fprintf(fp, "\n");
}

int pattern::is_valid (const char *str, int i_word, int i_bord) const
{
	int len = 0;

	for ( ; word[i_word] != '\0' ; ++i_word, ++len)
	{
		int res = 0;

		switch (spec[i_word])
		{
			case type_pattern::one:
				res = (str[len] != '\0');
				break;
			case type_pattern::many:
				return is_many(str + len, i_word + 1, i_bord); // Очень тупо, но что есть, то есть
			case type_pattern::out:
				res = is_out(str, len, i_bord);
				i_bord++;
				break;
			case type_pattern::in:
				res = is_in(str, len, i_bord);
				i_bord++;
				break;
			case type_pattern::none:
				res = (word[i_word] == str[len]);
				break;
		}
		
		if (!res)
			return 0;
	}

	if (str[len] != '\0')
		return 0;

	return 1;
}

