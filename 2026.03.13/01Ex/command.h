#ifndef COMMAND_H
#define COMMAND_H

#include "io_status.h"
#include "ordering.h"
#include "separator.h"
#include "condition.h"
#include "operation.h"
#include "command_type.h"
#include "comparator.h"
#include "validator.h"

#include "record.h"
#include "pattern.h"
#include "list_node.h"
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

		io_status parse (char *cmd);
		char * parse_command(char *cmd);

		// Parse commands
		io_status parse_insert (char *cmd);
		io_status parse_select (char *cmd);
		bool parse_delete (char *cmd);
		
		// Parse elems of command
		io_status parse_order (char *cmd);
		io_status parse_output (char *cmd);
		bool parse_search_terms (char *cmd);
		bool parse_search_term (char *string);
		
		// Parse condition
		bool parse_name (char *field, char *oper, char *value);
		bool parse_phone (char *field, char *oper, char *value);
		bool parse_group (char *field, char *oper, char *value);
		bool parse_condition (const char *oper, const fields_t field);

		void print (FILE *fp = stdout) const;
		
		int apply (list2<record> *worm);
		int apply_select (list2<record> *worm) const;
		void apply_delete (list2<record> *worm) const;

		list2_node<record> * validate (list2<record> *worm) const;
		void make_validator (validator<command, record>& val) const;
		bool is_true (record&) const;
		bool is_false (record&) const;

		bool cmp_group (record& x) const;
		bool cmp_phone (record& x) const;
		bool cmp_name(record& x) const;
		bool like_name (record& x) const;
		bool nlike_name (record& x) const;

		bool is_valid (const record& x) const;

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
						*funcs[i] = &record::cmp_phone;
						break;
					case ordering::group:
						*funcs[i] = &record::cmp_group;
						break;
					case ordering::none:
						*funcs[i] = &cmp_none;
						break;
				}
			}
		}

		static int cmp_none (const record&, const record&) { return 0; }
	protected:
		void erase ()
		{
			record::erase();
			pattern::erase();

			c_name 	= condition::none;
			c_phone	= condition::none;
			c_group	= condition::none;
			op = operation::none;
			type = command_type::none;

			for (int i = 0 ; i < max_items ; ++i)
				order_by[i] = order[i] = ordering::none;
		}
};

#endif // COMMAND_H
