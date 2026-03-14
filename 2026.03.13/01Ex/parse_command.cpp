#include "command.h"
#include "command_type.h"
#include "operation.h"
#include "separator.h"


io_status command::parse (char *cmd)
{
	erase();

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

char * command::parse_command(char *cmd)
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
		default:
			type = command_type::none;
			break;
	}

	cmd = separator::skip_spaces(cmd);
	return cmd;
}

io_status command::parse_insert (char *cmd)
{
	char buf[LEN] = {};
	if (sscanf(cmd, " ( %[^,] , %d , %d ) ", buf, &phone, &group) != 3)
		return io_status::format;

	word = std::make_unique<char[]>(strlen(buf) + 1);

	int i = 0;
	for (; buf[i] != '\0' ; ++i)
		word[i] = buf[i];
	word[i] = '\0';

	return io_status::success;
}

io_status command::parse_select (char *cmd)
{
	io_status ret = io_status::format;
	char *where = cmd,
		 *order_by = nullptr;
	bool had_where = false;

	if ((where = strstr(cmd, " where ")) != nullptr)
	{
		where[0] = '\0';
		where += 7;

		had_where = true;
	}

	if ((order_by = strstr(where, " order by ")) != nullptr)
	{
		order_by[0] = '\0';
		order_by += 10;
		
		if ((ret = parse_order(order_by)) != io_status::success)
			return ret;
	}

	if (had_where && (!parse_search_terms(where)))
		return io_status::format;

	if ((ret = parse_output(cmd)) != io_status::success)
		return ret;

	return io_status::success;
}

bool command::parse_delete (char *cmd)
{
	bool ret = true;

	if ((cmd = strstr(cmd, " where ")) != nullptr)
	{
		cmd += 7;

		if (!parse_search_terms(cmd))
			ret = false;
	}

	return ret;
}

io_status command::parse_order (char *cmd)
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


io_status command::parse_output (char *cmd)
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

bool command::parse_search_terms (char *cmd)
{
	char *subcmd;
	op = operation::lor;

	while ((subcmd = strstr(cmd, " and ")) != nullptr)
	{
		subcmd[0] = '\0';
		if (!parse_search_term(cmd))
			return false;

		op = operation::land;

		subcmd += 5;
		cmd = subcmd;
	}

	if (op == operation::land)
	{
		if (!parse_search_term(cmd))
			return false;

		return true;
	}

	while ((subcmd = strstr(cmd, " or ")) != nullptr)
	{
		subcmd[0] = '\0';
		if (!parse_search_term(cmd))
			return false;

		op = operation::lor;

		subcmd += 4;
		cmd = subcmd;
	}

	return parse_search_term(cmd);
}

bool command::parse_search_term (char *string)
{
	char field[separator::len_string] = {},
		 negation[separator::len_string] = {},
		 oper[separator::len_string] = {},
		 value[separator::len_string] = {};
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
		
bool command::parse_name (char *field, char *oper, char *value)
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

bool command::parse_phone (char *field, char *oper, char *value)
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

bool command::parse_group (char *field, char *oper, char *value)
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

bool command::parse_condition (const char *oper, const fields_t field)
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
