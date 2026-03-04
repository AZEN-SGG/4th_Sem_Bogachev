#ifndef PATTERN_H
#define PATTERN_H

#include "io_status.h"

#include <memory>
#include <cstdio>

#define LEN 1234

enum class type_pattern
{
	none,
	one,
	many,
	in,
	out,
};

struct borders
{
	char start;
	char end;
};

class pattern
{
	private:
		std::unique_ptr<char[]> word;
		std::unique_ptr<borders[]> span;
		std::unique_ptr<type_pattern[]> spec;
	public:
		char * get_word () const { return word.get(); }
		type_pattern * get_spec () const { return spec.get(); }
		borders * get_span () const { return span.get(); }
	
		io_status read (char *str)
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

			for (int i = 0 ; i < b_len ; ++i)
				span[i] = b_buf[i];

			return io_status::success;
		}

		void print (FILE *fp) const
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

		int is_valid (const char *str, int i_word, int i_bord) const
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
	private:
		int is_many (const char *str, int i_word, int i_bord)
		{
			int len = 0;

			do
			{
				int res = is_valid (str + len, i_word, i_bord);
				if (res)
					return res;
			} while (str[len++] != '\0');

			return 0;
		}

		int is_in (const char *str, int str_i, int bord_i)
		{
			return ((str[str_i] >= span[bord_i].start) &&
					(str[str_i] <= span[bord_i].end));
		}

		int is_out (const char *str, int str_i, int bord_i)
		{
			return ((str[str_i] < span[bord_i].start) ||
					(str[str_i] > span[bord_i].end));
		}

		io_status process_slash (char *str, int *p_len, int *p_back, type_pattern *p_buf)
		{
			str[*p_len - *p_back] = str[*p_len + 1];
			p_buf[*p_len - *p_back] = type_pattern::none;
			(*p_len)++;
			(*p_back)++;

			if (str[*p_len] == '\0')
				return io_status::format;

			return io_status::success;
		}

		io_status process_percent (char *str, int *p_len, int *p_back, type_pattern *p_buf)
		{
			str[*p_len - *p_back] = '%';
			p_buf[*p_len - *p_back] = type_pattern::many;

			return io_status::success;
		}

		io_status process_underscore (char *str, int *p_len, int *p_back, type_pattern *p_buf)
		{
			str[*p_len - *p_back] = '_';
			p_buf[*p_len - *p_back] = type_pattern::one;

			return io_status::success;
		}

		io_status process_brackets (char *str, int *p_len, int *p_back, type_pattern *p_buf, borders *b_buf, int *p_bord_len)
		{
			int stage = 1;
			
			// is in or out?
			if (str[*p_len + stage] == '^')
			{
				p_buf[*p_len - *p_back] = type_pattern::out;
				stage++;
			} else
				p_buf[*p_len - *p_back] = type_pattern::in;

			// from
			stage += (str[*p_len + stage] == '\\');
			b_buf[*p_bord_len].start = str[*p_len + stage];
			stage++;

			// dash
			stage += (str[*p_len + stage] == '\\');
			if (str[*p_len + stage] != '-')
				return io_status::format;
			stage++;

			// to
			stage += (str[*p_len + stage] == '\\');
			b_buf[*p_bord_len].end = str[*p_len + stage];
			stage++;
			
			if (str[*p_len + stage] != ']')
				return io_status::format;
			
			(*p_back) += stage;
			(*p_len) += stage;
			(*p_bord_len)++;

			return io_status::success;
		}

		io_status parsing_pattern (char *str, type_pattern *p_buf, int *p_len, borders *b_buf, int *p_bord_len)
		{
			io_status ret;
			int len = 0,
				back = 0;

			(*p_bord_len) = 0;
			
			for (; str[len] != '\0' ; len++)
			{
				switch (str[len])
				{
					case '\\':
						ret = process_slash(str, &len, &back, p_buf);
						break;
					case '%':
						ret = process_percent(str, &len, &back, p_buf);
						break;
					case '_':
						ret = process_underscore(str, &len, &back, p_buf);
						break;
					case '[':
						ret = process_brackets(str, &len, &back, p_buf, b_buf, p_bord_len);
						break;
					default:
						str[len - back] = str[len];
						p_buf[len - back] = type_pattern::none;
						ret = io_status::success;
				}

				if (ret != io_status::success)
					return ret;
			}

			str[len - back] = '\0';
			(*p_len) = len - back;

			return io_status::success;
		}
};

#endif // PATTERN_H
