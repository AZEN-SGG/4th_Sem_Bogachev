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
#include "search_conditions.h"
#include "database.h"

#include <cstdio>
#include <memory>

class request;

enum class fields_t
{
	name,
	phone,
	group,
};

class command : public search_conditions<name_query>
{
private:
	static const int len_command = 3;
	static const int max_items = 3;
	command_type type	= command_type::none;
	ordering order[max_items] = {};
	ordering order_by[max_items] = {};
public:
	command () = default;
	~command () = default;

	command& operator= (command&& x)
	{
		static_cast<name_query&>(*this) = static_cast<name_query&&>(x);

		type = x.type;
		for (int i = 0 ; i < max_items ; ++i)
		{
			order[i] = x.order[i];
			order_by[i] = x.order_by[i];
		}

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
	static void print_condition (FILE *fp, const condition cond);
	static void print_operation (FILE *fp, const operation op);
	
	io_status apply (database<record> *db, int *res);
	io_status apply_insert (database<record> *db);
	int apply_select (database<record> *db);
	void apply_delete (database<record> *db);

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
