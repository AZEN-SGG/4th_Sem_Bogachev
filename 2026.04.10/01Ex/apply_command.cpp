#include "command.h"
#include "pattern.h"
#include "record.h"
#include "search_conditions.h"
#include "validator.h"


int command::apply (database<record> *db)
{
	int res = 0;

	switch (type)
	{
		case command_type::insert:
			apply_insert(db);
			break;
		case command_type::select:
			res = apply_select(db);
			break;
		case command_type::del:
			apply_delete(db);
			break;
		case command_type::quit:
			break;
		case command_type::none:
			break;
	}

	return res;
}

// Бинарный поиск одинакового элемента при добавлении
void command::apply_insert (database<record> *db)
{
	auto node = db->search_node(static_cast<const record&>(*this));
	if (!node)
		db->add(static_cast<record &&>(*this));
}

int command::apply_select (database<record> *db)
{
	auto curr = db->validate(static_cast<search_conditions<name_query>&>(*this));

	if (order_by[0] != ordering::none)
	{
		// Create cmp func
		comparator<record> comp;
		make_cmp(comp);

		// Sort list
		curr = db->sort_selected(curr, comp);
	}

	// Print
	return db->print_selected(curr, stdout, order);
}

void command::apply_delete (database<record> *db)
{
	auto curr = db->validate(static_cast<search_conditions<name_query>&>(*this));
	db->delete_selected(curr);
}

template <>
template <>
void search_conditions<name_query>::make_validator (validator<search_conditions<name_query>, record>& val) const
{
	val.op = op;

	validator<search_conditions<name_query>, record>::valid_t neutral = nullptr;
	switch (op)
	{
		case operation::land:
			neutral = &record::is_true;
			break;
		case operation::lor:
			neutral = &record::is_false;
			break;
		case operation::none:
			neutral = nullptr;
			break;
	}

	if (c_group != condition::none)
		val.fgroup = &record::cmp_group;
	else
		val.fgroup = neutral;

	if (c_phone != condition::none)
		val.fphone = &record::cmp_phone;
	else
		val.fphone = neutral;

	if (c_name != condition::none)
	{
		if (c_name == condition::like)
			val.fname = &pattern::like_name;
		else if (c_name == condition::nlike)
			val.fname = &pattern::nlike_name;
		else
			val.fname = &command::cmp_name;
	} else
		val.fname = neutral;
}

