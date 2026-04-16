#include "command.h"
#include "io_status.h"
#include "pattern.h"
#include "record.h"
#include "search_conditions.h"
#include "validator.h"


io_status command::apply (database<record> *db, int *res, FILE *fp)
{
	io_status ret = io_status::success;

	switch (type)
	{
		case command_type::insert:
			ret = apply_insert(db);
			break;
		case command_type::select:
			(*res) += apply_select(db, fp);
			break;
		case command_type::del:
			apply_delete(db);
			break;
		case command_type::quit:
			break;
		case command_type::none:
			break;
	}

	return ret;
}

// Бинарный поиск одинакового элемента при добавлении
io_status command::apply_insert (database<record> *db)
{
	io_status ret = io_status::success;

	auto node = db->search_node(static_cast<const record&>(*this));
	if (!node)
		ret = db->add(static_cast<record&&>(*this));

	return ret;
}

int command::apply_select (database<record> *db, FILE *fp)
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
	return db->print_selected(curr, fp, order);
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
		val.fgroup = &search_conditions<name_query>::cmp_group;
	else
		val.fgroup = neutral;

	if (c_phone != condition::none)
		val.fphone = &search_conditions<name_query>::cmp_phone;
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

