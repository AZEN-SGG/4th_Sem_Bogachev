#include "command.h"


int command::apply (list2<record> *worm)
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

int command::apply_select (list2<record> *worm) const
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

void command::apply_delete (list2<record> *worm) const
{
	auto curr = worm->select_valid(*this);
	worm->delete_sublist(curr);
}

bool command::is_valid (const record& x) const
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
	
