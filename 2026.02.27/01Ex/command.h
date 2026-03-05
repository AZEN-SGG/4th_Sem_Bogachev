#ifndef COMMAND_H
#define COMMAND_H

#include "condition.h"
#include "record.h"
#include "pattern.h"

#include <cstdio>


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
	public:
		command () = default;
		~command () = default;

		bool parse (char *string)
		{
			erase();

			char field[LEN] = {}, oper[LEN] = {}, value[LEN] = {};
			if (sscanf(string, "%s%s%s", field, oper, value) != 3)
				return false;

			switch (field[0])
			{
				case 'n':
					return parse_name(field, oper, value);
				case 'p':
					return parse_phone(field, oper, value);
				case 'g':
					return parse_group(field, oper, value);
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
				else
					res = x.compare_word(c_name, *this);
			}

			return res;
		}
	protected:
		void erase ()
		{
			record::erase();
			pattern::erase();

			c_name 	= condition::none;
			c_phone	= condition::none;
			c_group	= condition::none;
		}
};

#endif // COMMAND_H
