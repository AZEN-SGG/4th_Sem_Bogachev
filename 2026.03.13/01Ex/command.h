#ifndef COMMAND_H
#define COMMAND_H

#include "io_status.h"
#include "ordering.h"
#include "separator.h"
#include "condition.h"
#include "operation.h"
#include "command_type.h"
#include "comparator.h"

#include "record.h"
#include "pattern.h"
#include "list.h"

#include <cstdio>
#include <memory>

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
		static const int len_command = 3;
		static const int max_items = 3;
		command_type type	= command_type::none;
		condition c_name	= condition::none;
		condition c_phone	= condition::none;
		condition c_group	= condition::none;
		operation op		= operation::none;
		ordering order[max_items] = {};
		ordering order_by[max_items] = {};
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

		command_type get_type () const { return type; }

		io_status parse (char *cmd)
		{
			io_status ret = io_status::success;
			cmd = parse_command(cmd);
			switch (type)
			{
				case command_type::insert:
					ret = parse_insert(cmd);
					break;
				case command_type::select:
					ret = parse_select(cmd);
					break;
				case command_type::del:
					if (!parse_delete(cmd))
						ret = io_status::format;
					break;
				case command_type::quit:
					ret = io_status::success;
					break;
				case command_type::none:
					ret = io_status::format;
					break;
			}

			return ret;
		}

		char * parse_command(char *cmd)
		{
			cmd = separator::skip_spaces(cmd);
			
			switch (cmd[0])
			{
				case 'q':
					if ((cmd = strstr(cmd, "quit")) != nullptr)
					{
						cmd += 4;
						type = command_type::quit;
					}
					break;
				case 'i':
					if ((cmd = strstr(cmd, "insert")) != nullptr)
					{
						cmd += 6;
						type = command_type::insert;
					}
					break;
				case 's':
					if ((cmd = strstr(cmd, "select")) != nullptr)
					{
						cmd += 6;
						type = command_type::select;
					}
					break;
				case 'd':
					if ((cmd = strstr(cmd, "delete")) != nullptr)
					{
						cmd += 6;
						type = command_type::del;
					}
					break;
			}

			return cmd;
		}

		io_status parse_insert (char *cmd)
		{
			char buf[LEN] = {};
			if (sscanf(cmd, "(%s,%d,%d)", buf, &phone, &group) != 3)
				return io_status::format;

			word = std::make_unique<char[]>(strlen(buf));

			int i = 0;
			for (; buf[i] != '\0' ; ++i)
				word[i] = buf[i];
			word[i] = '\0';

			return io_status::success;
		}

		io_status parse_select (char *cmd)
		{
			io_status ret = io_status::format;
			char *where = nullptr,
				 *in_order = nullptr;
			int len = 0;
			for (; cmd[len] != '\0' && len < 9 ; ++len);
			for (; cmd[len] != '\0' ; ++len)
			{
				if (
						(!where) &&
						separator::contains(cmd[len - 6]) &&
						cmd[len - 5] == 'w' &&
						cmd[len - 4] == 'h' &&
						cmd[len - 3] == 'e' &&
						cmd[len - 2] == 'r' &&
						cmd[len - 1] == 'e' &&
						separator::contains(cmd[len])
				) {
					where = cmd + len - 6;
					where[0] = '\0';
					where += 7;
				} else if (
						separator::contains(cmd[len - 9]) &&
						cmd[len - 8] == 'o' &&
						cmd[len - 7] == 'r' &&
						cmd[len - 6] == 'd' &&
						cmd[len - 5] == 'e' &&
						cmd[len - 4] == 'r' &&
						separator::contains(cmd[len - 3]) &&
						cmd[len - 2] == 'b' &&
						cmd[len - 1] == 'y' &&
						separator::contains(cmd[len])
				) {
					in_order = cmd + len - 9;
					in_order[0] = '\0';
					in_order += 10;

					if ((ret = parse_order(in_order)) != io_status::success)
						return ret;
					break;
				}
			}

			if ((ret = parse_output(cmd)) != io_status::success)
				return ret;

			if (where && (!parse_search_terms(where)))
				return io_status::format;

			return io_status::success;
		}

		bool parse_delete (char *cmd)
		{
			bool ret = true;

			cmd = separator::skip_spaces(cmd);
			if (
				cmd[0] == 'w' &&
				cmd[1] == 'h' &&
				cmd[2] == 'e' &&
				cmd[3] == 'r' &&
				cmd[4] == 'e' &&
				separator::contains(cmd[5])
			) {
				cmd += 6;
				ret = parse_search_terms(cmd);
			} else
			{
				c_name = condition::none;
				c_phone = condition::none;
				c_group = condition::none;
			}

			return ret;
		}

		io_status parse_order (char *cmd)
		{
			char *saveptr = nullptr;

			int len = 0;
			while ((cmd = strtok_r(cmd, ", ", &saveptr)) && (len < max_items))
			{
				switch (cmd[0])
				{
					case 'n':
						if (strncmp(cmd, "name", 5) == 0)
							order_by[len] = ordering::name;
						break;
					case 'p':
						if (strncmp(cmd, "phone", 6) == 0)
							order_by[len] = ordering::phone;
						break;
					case 'g':
						if (strncmp(cmd, "group", 6) == 0)
							order_by[len] = ordering::group;
						break;
					default:
						return io_status::format;
				}

				len++;
				cmd = nullptr;
			}

			for (int i = len ; i < max_items; ++i)
				order[i] = ordering::none;

			return io_status::success;
		}


		io_status parse_output (char *cmd)
		{
			char *saveptr = nullptr;

			if (cmd[0] == '*' && cmd[1] == '\0')
			{
				order[0] = ordering::name;
				order[1] = ordering::phone;
				order[2] = ordering::group;
				return io_status::success;
			}

			int len = 0;
			while ((cmd = strtok_r(cmd, ", ", &saveptr)) && (len < max_items))
			{
				switch (cmd[0])
				{
					case 'n':
						if (strncmp(cmd, "name", 5) == 0)
							order[len] = ordering::name;
						break;
					case 'p':
						if (strncmp(cmd, "phone", 6) == 0)
							order[len] = ordering::phone;
						break;
					case 'g':
						if (strncmp(cmd, "group", 6) == 0)
							order[len] = ordering::group;
						break;
					default:
						return io_status::format;
				}

				len++;
				cmd = nullptr;
			}

			for (int i = len ; i < max_items; ++i)
				order[i] = ordering::none;

			return io_status::success;
		}

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

		void make_cmp (comparator<record>& x) const
		{
			comparator<record>::cmp_t *funcs[] = {&x.first, &x.second, &x.third};

			for (int i = 0 ; i < max_items ; ++i)
			{
				switch (order_by[i])
				{
					case ordering::name:
						*funcs[i] = &cmp_word;
						break;
					case ordering::phone:
						*funcs[i] = &cmp_phone;
						break;
					case ordering::group:
						*funcs[i] = &cmp_group;
						break;
					case ordering::none:
						*funcs[i] = &cmp_none;
						break;
				}
			}
		}

		static int cmp_none (const record&, const record&) { return 0; }

		int apply (list2<record> *worm)
		{
			int res = 0;

			switch (type)
			{
				case command_type::insert:
					worm->add_node(static_cast<record&&>(*this));
					break;
				case command_type::select:
					res = apply_select(worm);
					break;
				case command_type::del:
					apply_delete(worm);
					break;
				case command_type::quit:
					break;
				case command_type::none:
					break;
			}

			return res;
		}

		int apply_select (list2<record> *worm) const
		{
			// Create sublist 
			auto curr = worm->select_valid(*this);

			// Create cmp func
			comparator<record> comp;
			make_cmp(comp);

			// Sort list
			curr = worm->sort(curr, comp);

			// Print
			return worm->print_sublist(curr);
		}

		void apply_delete (list2<record> *worm) const
		{
			auto curr = worm->select_valid(*this);
			worm->delete_sublist(curr);
		}

		bool is_valid (const record& x) const
		{
			bool res = false;
			if (c_group != condition::none)
				res = x.compare_group(c_group, *this);
			if (c_phone != condition::none)
			{
				switch (op)
				{
					case operation::land:
						res = res && x.compare_phone(c_phone, *this);
						break;
					case operation::lor:
						res = res || x.compare_phone(c_phone, *this);
						break;
					case operation::none:
						res = x.compare_phone(c_phone, *this);
						break;
				}
			}
			if (c_name != condition::none)
			{
				bool temp = false;
				if (c_name == condition::like)
					temp = pattern::is_valid(x.get_word(), 0, 0);
				else if (c_name == condition::nlike)
					temp = !pattern::is_valid(x.get_word(), 0, 0);
				else
					temp = x.compare_word(c_name, *this);

				switch (op)
				{
					case operation::land:
						res = res && temp;
						break;
					case operation::lor:
						res = res || temp;
						break;
					case operation::none:
						res = temp;
						break;
				}
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
			op = operation::none;
		}
};

#endif // COMMAND_H
