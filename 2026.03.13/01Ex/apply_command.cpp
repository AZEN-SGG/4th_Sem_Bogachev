#include "command.h"
#include "condition.h"
#include "operation.h"
#include "ordering.h"
#include "validator.h"


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
	validate();

	// Create sublist 
	auto curr = worm->select_valid(*this);

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
	auto curr = worm->select_valid(*this);
	worm->delete_sublist(curr);
}

list2_node * command::validation ()
{
	validator<command, record> val;
	val.op = op;

	validator<command, record>::valid_t neutral = nullptr;
	switch (op)
	{
		case operation::land:
			neutral = &is_true;
		case operation::lor:
			neutral = &is_false;
		case operation::none:
			neutral = &is_true;
	}

	if (c_group != condition::none)
	{
		r_group = x.compare_group(c_group, *this);
		if ((!r_group) && op == operation::land)
			return false;
	} else
}

bool is_true (record&) { return true; }
bool is_false (record&) { return false; }

bool command::is_valid (const record& x) const
{
	bool r_group = false,
		 r_phone = false,
		 r_name = false;

	{
		if (op == operation::land)
			r_group = true;
		else
			r_group = false;
	}

	if (c_phone != condition::none)
	{
		r_phone = x.compare_phone(c_phone, *this);
		if ((!r_phone) && op == operation::land)
			return false;
	} else
	{
		if (op == operation::land)
			r_phone = true;
		else
			r_phone = false;
	}

	if (c_name != condition::none)
	{
		if (c_name == condition::like)
			r_name = pattern::is_valid(x.get_word(), 0, 0);
		else if (c_name == condition::nlike)
			r_name = !pattern::is_valid(x.get_word(), 0, 0);
		else
			r_name = x.compare_word(c_name, *this);
	} else
	{
		if (op == operation::land)
			r_name = true;
		else
			r_name = false;
	}

	switch (op)
	{
		case operation::land:
			return r_name && r_phone && r_group;
		default:
			if (
				c_name == condition::none &&
				c_phone == condition::none &&
				c_group == condition::none
			) {
				r_name = true;
			}
			return r_name || r_phone || r_group;
	}
}
	
