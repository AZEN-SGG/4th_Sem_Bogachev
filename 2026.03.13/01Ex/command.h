#ifndef COMMAND_H
#define COMMAND_H

#include "separator.h"
#include "parse_status.h"
#include "condition.h"
#include "operation.h"
#include "record.h"
#include "pattern.h"

#include <cstdio>

class request;

enum class fields_t
{
	name,
	phone,
	group,
};


class command : public record, public pattern
{
	private:
		condition c_name	= condition::none;
		condition c_phone	= condition::none;
		condition c_group	= condition::none;
		operation op		= operation::none;
		static const int len_command = 3;
	public:
		command () = default;
		~command () = default;

		command& operator= (command&& x)
		{
			if (this == &x)
				return (*this);

			word = std::move(x.word);
			span = std::move(x.span);
			spec = std::move(x.spec);
			group = x.group;
			phone = x.phone;
			
			c_name = x.c_name;
			c_phone = x.c_phone;
			c_group = x.c_group;
			op = x.op;

			x.erase();

			return (*this);
		}
		command& operator= (const command& x) = delete;

		bool parse_search_terms (char *cmd)
		{
			int len = 0;
			for (; cmd[len] != '\0' && len < 4 ; ++len);
			if (cmd[len] == '\0')
				return false;

			int len_c = 1;
			for (; cmd[len] != '\0' && len_c < len_command ; ++len)
			{
				if (
					 separator::contains(cmd[len - 4]) &&
					 cmd[len - 3] == 'a' &&
					 cmd[len - 2] == 'n' &&
					 cmd[len - 1] == 'd' &&
					 separator::contains(cmd[len])
				) {
					len_c++;
					op = operation::land;
					cmd[len - 4] = '\0';
					if (!parse_search_term(cmd))
						return false;
					cmd += len;
					len = 0;
				} else if (
					 separator::contains(cmd[len - 3]) &&
					 cmd[len - 2] == 'o' &&
					 cmd[len - 1] == 'r' &&
					 separator::contains(cmd[len])	
				) {
					len_c++;
					op = operation::lor;
					cmd[len - 3] = '\0';
					if (!parse_search_term(cmd))
						return false;
					cmd += len;
					len = 0;
				} else
					break;
			}

			return parse_search_term(cmd);
		}

		bool parse_search_term (char *string)
		{
			char field[separator::len_string] = {}, negation[] = {}, oper[LEN] = {}, value[LEN] = {};
			int n = sscanf(string, "%s%s%s%s", field, negation, oper, value);
			if (n != 3 && n != 4)
				return false;

			if ((n == 4) && std::strncmp(negation, "not", 4) == 0)
			{
				if (std::strncmp(oper, "like", 5) == 0)
				{
					if (std::strncmp(field, "name", 5) != 0)
						return false;

					if (read_word(value) != io_status::success)
						return false;

					c_name = condition::nlike;
					
					return true;
				} else
					return false;
			} else if (n == 4)
				return false;

			switch (field[0])
			{
				case 'n':
					return parse_name(field, negation, oper);
				case 'p':
					return parse_phone(field, negation, oper);
				case 'g':
					return parse_group(field, negation, oper);
				default:
					return false;
			}
		}

		bool parse_name (char *field, char *oper, char *value)
		{
			if (std::strncmp(field, "name", 5) != 0)
				return false;

			if (!parse_condition(oper, fields_t::name))
				return false;

			switch (c_name)
			{
				case condition::none:
					return false;
				case condition::like:
					if (read_word(value) != io_status::success)
						return false;
					break;
				default:
					init(value, 0, 0);
			}

			return true;
		}

		bool parse_phone (char *field, char *oper, char *value)
		{
			if (std::strncmp(field, "phone", 6) != 0)
				return false;

			if (!parse_condition(oper, fields_t::phone))
				return false;

			bool res;
			switch (c_phone)
			{
				case condition::none:
					res = false;
					break;
				case condition::like:
					res = false;
					break;
				default:
					res = (sscanf(value, "%d", &phone) == 1);
					break;
			}

			return res;
		}

		bool parse_group (char *field, char *oper, char *value)
		{
			if (std::strncmp(field, "group", 6) != 0)
				return false;

			if (!parse_condition(oper, fields_t::group))
				return false;

			bool res;
			switch (c_group)
			{
				case condition::none:
					res = false;
					break;
				case condition::like:
					res = false;
					break;
				default:
					res = (sscanf(value, "%d", &group) == 1);
					break;
			}

			return res;
		}

		bool parse_condition (const char *oper, const fields_t field)
		{
			condition cond = condition::none;

			if (oper[1] == '\0')
			{
				switch (oper[0])
				{
					case '<':
						cond = condition::lt;
						break;
					case '>':
						cond = condition::gt;
						break;
					case '=':
						cond = condition::eq;
						break;
					default:
						return false;
				}
			} else if (oper[1] == '=')
			{
				if (oper[0] == '<')
						cond = condition::le;
				else if (oper[0] == '>')
						cond = condition::ge;
				else
					return false;
			} else if (oper[0] == '<' && oper[1] == '>')
				cond = condition::ne;
			else if (std::strncmp(oper, "like", 5) == 0)
				cond = condition::like;
			else
				return false;

			switch (field)
			{
				case fields_t::name:
					c_name = cond;
					break;
				case fields_t::phone:
					c_phone = cond;
					break;
				case fields_t::group:
					c_group = cond;
					break;
			}

			return true;
		}

		void print (FILE *fp = stdout) const
		{
			condition cond = condition::none;
			if (c_group != condition::none)
			{
				fprintf(fp, "%d", group);
				cond = c_group;
			} else if (c_phone != condition::none)
			{
				fprintf(fp, "%d", phone);
				cond = c_phone;
			} else if (c_name != condition::none)
			{
				fprintf(fp, "%s", word.get());
				cond = c_name;
			} else
				fprintf(fp, "none");

			switch (cond)
			{
				case condition::none:
					fprintf(fp, " none\n\n");
					break;
				case condition::lt:
					fprintf(fp, " <\n\n");
					break;
				case condition::gt:
					fprintf(fp, " >\n\n");
					break;
				case condition::le:
					fprintf(fp, " <=\n\n");
					break;
				case condition::ge:
					fprintf(fp, " >=\n\n");
					break;
				case condition::eq:
					fprintf(fp, " =\n\n");
					break;
				case condition::ne:
					fprintf(fp, " <>\n\n");
					break;
				case condition::like:
					fprintf(fp, " like\n\n");
					break;
				case condition::nlike:
					fprintf(fp, " not like\n\n");
					break;
			}
		}

		bool apply (const record& x) const
		{
			bool res = false;
			if (c_group != condition::none)
				res = x.compare_group(c_group, *this);
			else if (c_phone != condition::none)
				res = x.compare_phone(c_phone, *this);
			else if (c_name != condition::none)
			{
				if (c_name == condition::like)
					res = is_valid(x.get_word(), 0, 0);
				else if (c_name == condition::nlike)
					res = !is_valid(x.get_word(), 0, 0);
				else
					res = x.compare_word(c_name, *this);
			}

			return res;
		}

		friend class request;
	protected:
		void erase ()
		{
			record::erase();
			pattern::erase();

			c_name 	= condition::none;
			c_phone	= condition::none;
			c_group	= condition::none;
			op = operation::none;
		}
};

#endif // COMMAND_H
