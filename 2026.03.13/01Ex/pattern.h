#ifndef PATTERN_H
#define PATTERN_H

#include "io_status.h"

#include <memory>
#include <cstdio>

#define LEN 1234

class name_t
{
	protected:
		std::unique_ptr<char[]> word = nullptr;

		void erase () { word.reset(); }
	public:
		name_t () = default;
		~name_t () = default;

		name_t(const name_t&) = delete;
		name_t& operator=(const name_t&) = delete;

		name_t(name_t&&) noexcept = default;
		name_t& operator=(name_t&&) noexcept = default;
};

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

class pattern : virtual public name_t
{
	protected:
		std::unique_ptr<borders[]> span = nullptr;
		std::unique_ptr<type_pattern[]> spec = nullptr;
	public:
		pattern () = default;
		~pattern () = default;

		pattern (const pattern&) = delete;

		pattern& operator= (pattern&& x)
		{
			if (this == &x)
				return (*this);

			word = std::move(x.word);
			span = std::move(x.span);
			spec = std::move(x.spec);

			return (*this);
		}
		pattern& operator= (const pattern& x) = delete;

		char * get_word () const { return word.get(); }
		type_pattern * get_spec () const { return spec.get(); }
		borders * get_span () const { return span.get(); }
	
		io_status read_word (char *str);
		void print (FILE *fp) const;
		int is_valid (const char *str, int i_word, int i_bord) const;
	private:
		int is_many (const char *str, int i_word, int i_bord) const
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

		int is_in (const char *str, int str_i, int bord_i) const
		{
			return (str[str_i] != '\0' &&
					((str[str_i] >= span[bord_i].start) &&
					(str[str_i] <= span[bord_i].end)));
		}

		int is_out (const char *str, int str_i, int bord_i) const
		{
			return (str[str_i] != '\0' && 
					((str[str_i] < span[bord_i].start) ||
					(str[str_i] > span[bord_i].end)));
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
	protected:
		void erase ()
		{
			name_t::erase();
			span.reset();
			spec.reset();
		}
};

#endif // PATTERN_H
