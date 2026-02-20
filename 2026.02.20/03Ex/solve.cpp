#include "solve.h"


io_status replace_words (FILE *in, FILE *out, const avl_tree<pair> *olha, const bool hash[], int *res)
{
	char word[LEN] = {},
		 buffer[LEN] = {};

	*res = 0;

	while (fgets(buffer, LEN, in) != nullptr)
	{
		int len = 0,
			l_word = 0;
		// Идём по строке
		for (len = 0 ; buffer[len] != '\0' ; len++)
		{
			// Если символ пробельный
			if (hash[static_cast<unsigned char>(buffer[len])] || buffer[len] == '\n')
			{
				// Если перед ним было слово
				if (l_word)
				{
					// Конец слова
					word[l_word] = '\0';

					avl_tree_node<pair> *node;
					if ((node = olha->find(word)) != nullptr)
					{
						if (node->get_value() != nullptr)
							fprintf(out, "%s", node->get_value());
					} else
						fprintf(out, "%s", word);

					l_word = 0;
				}

				fprintf(out, "%c", buffer[len]);
			} else
			{
				word[l_word] = buffer[len];
				l_word++;
			}
		}
		
		if (l_word)
		{
			word[l_word] = '\0';

			avl_tree_node<pair> *node;
			if ((node = olha->find(word)) != nullptr)
			{
				if (node->get_value() != nullptr)
					fprintf(out, "%s", node->get_value());
			} else
				fprintf(out, "%s", word);

			l_word = 0;
		}

		(*res)++;
	}

	if (feof(in) == 0)
		return io_status::read;

	return io_status::success;
}

io_status solve_03 (char *f_in, char *f_out, char *s, char *x, const char *t, int *r)
{
	auto olha = std::make_unique<avl_tree<pair>>();
    if (olha == nullptr)
		return io_status::memory;

	io_status ret = olha->read(s, x, t);
	if (ret != io_status::success)
		return ret;

// Отладочная
//	olha->print(100);

	FILE *in, *out;

	if ((in = fopen(f_in, "r")) == nullptr)
		return io_status::open;

	if ((out = fopen(f_out, "w")) == nullptr)
	{
		fclose(in);
		return io_status::open;
	}

	bool hash[CHAR_SIZE] = {};
	for (int i = 0 ; t[i] != '\0' ; i++)
		hash[static_cast<unsigned char>(t[i])] = true;

	ret = replace_words(in, out, olha.get(), hash, r);
	
	fclose(in);
	fclose(out);

	return io_status::success;
}
