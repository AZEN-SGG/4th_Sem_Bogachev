#include "command.h"

#include <cstdio>


void command::print (FILE *fp) const
{
	switch (type)
	{
		case command_type::select:
			fprintf(fp, "select");
			break;
		case command_type::insert:
			fprintf(fp, "insert");
			break;
		case command_type::del:
			fprintf(fp, "delete");
			break;
		case command_type::quit:
			fprintf(fp, "quit");
			break;
		case command_type::none:
			fprintf(fp, "none");
			break;
	}

	if (type == command_type::insert)
		fprintf(fp, " (%s, %d, %d)", word.get(), phone, group);

	if (type == command_type::select)
	{
		for (int i = 0 ; i < command::max_items ; ++i)
			switch (order[i])
			{
				case ordering::name:
					fprintf(fp, " name");
					break;
				case ordering::phone:
					fprintf(fp, " phone");
					break;
				case ordering::group:
					fprintf(fp, " group");
					break;
				case ordering::none:
					fprintf(fp, " none");
					break;
			}
	}

	if (c_group != condition::none ||
		c_phone != condition::none ||
		c_name != condition::none)
		fprintf(fp, " where");

	if (c_name != condition::none)
	{
		print_operation(fp, op);
		fprintf(fp, " name");
		print_condition(fp, c_name);
		fprintf(fp, " %s", word.get());
	}

	if (c_phone != condition::none)
	{
		print_operation(fp, op);
		fprintf(fp, " phone");
		print_condition(fp, c_phone);
		fprintf(fp, " %d", phone);
	}

	if (c_group != condition::none)
	{
		print_operation(fp, op);
		fprintf(fp, " group");
		print_condition(fp, c_group);
		fprintf(fp, " %d", group);
	}

	if (type == command_type::select)
	{
		fprintf(fp, " order by");

		for (int i = 0 ; i < command::max_items ; ++i)
			switch (order_by[i])
			{
				case ordering::name:
					fprintf(fp, " name");
					break;
				case ordering::phone:
					fprintf(fp, " phone");
					break;
				case ordering::group:
					fprintf(fp, " group");
					break;
				case ordering::none:
					fprintf(fp, " none");
					break;
			}
	}

	fprintf(fp, ";\n");
}

void command::print_condition (FILE *fp, const condition cond)
{
	switch (cond)
	{
		case condition::none:
			fprintf(fp, " none");
			break;
		case condition::lt:
			fprintf(fp, " <");
			break;
		case condition::gt:
			fprintf(fp, " >");
			break;
		case condition::le:
			fprintf(fp, " <=");
			break;
		case condition::ge:
			fprintf(fp, " >=");
			break;
		case condition::eq:
			fprintf(fp, " =");
			break;
		case condition::ne:
			fprintf(fp, " <>");
			break;
		case condition::like:
			fprintf(fp, " like");
			break;
		case condition::nlike:
			fprintf(fp, " not like");
			break;
	}
}

void command::print_operation (FILE *fp, const operation op)
{
	switch (op)
	{
		case operation::land:
			fprintf(fp, " and");
			break;
		case operation::lor:
			fprintf(fp, " or");
			break;
		case operation::none:
			fprintf(fp, " none");
			break;
	}
}
