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
	auto curr = validate(worm);

	if (order_by[0] != ordering::none)
	{
		// Create cmp func
		comparator<record> comp;
		make_cmp(comp);

		// Sort list
		curr = worm->sort(curr, comp);
	}

	// Print
	return worm->print_sublist(curr, stdout, order);
}

void command::apply_delete (list2<record> *worm) const
{
	auto curr = validate(worm);
	worm->delete_sublist(curr);
}

list2_node<record> * command::validate (list2<record> *worm) const
{
	validator<command, record> val;
	make_validator(val);

	// Create sublist 
	auto curr = worm->select_valid(*this, val);

	return curr;
}

void command::make_validator (validator<command, record>& val) const
{
	val.op = op;

	validator<command, record>::valid_t neutral = nullptr;
	switch (op)
	{
		case operation::land:
			neutral = &command::is_true;
		case operation::lor:
			neutral = &command::is_false;
		case operation::none:
			neutral = nullptr;
	}

	if (c_group != condition::none)
		val.fgroup = &command::cmp_group;
	else
		val.fgroup = neutral;

	if (c_phone != condition::none)
		val.fphone = &command::cmp_phone;
	else
		val.fgroup = neutral;

	if (c_name != condition::none)
	{
		if (c_name == condition::like)
			val.fname = &command::like_name;
		else if (c_name == condition::nlike)
			val.fname = &command::nlike_name;
		else
			val.fname = &command::cmp_name;
	} else
		val.fname = neutral;
}

bool command::is_true (record&) const { return true; }
bool command::is_false (record&) const { return false; }

bool command::cmp_group (record& x) const { return x.compare_group(c_group, *this); }
bool command::cmp_phone (record& x) const { return x.compare_phone(c_group, *this); }
bool command::cmp_name (record& x) const { return x.compare_word(c_group, *this); }
bool command::like_name (record& x) const { return pattern::is_valid(x.get_word(), 0, 0); }
bool command::nlike_name (record& x) const { return !pattern::is_valid(x.get_word(), 0, 0); }

