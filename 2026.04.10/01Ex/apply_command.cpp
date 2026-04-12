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
	// Получили head подсписка
	auto curr = validate(db);

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

list2_node<record> * command::validate (database<record> *db)
{
	list2_node<record> 	*curr = nullptr,
						*last = nullptr;

	validator<command, record> val;
	if (c_name == condition::eq
		&& c_phone == condition::eq
		&& c_group == condition::none
		&& op == operation::lor
	) {
		auto suit_phone = db->search_node<order, ordering::phone>(*this);
		// Нашли узел с таким телефоном
		if (suit_phone)
			curr = suit_phone->select_all(&last);

		auto suit_name = db->search_node<record>(*this);
		// Нашли узел с таким именем, берём все элементы
		if (suit_name)
		{
			list2_node<record> *temp = nullptr,
				*curr_by_name = suit_name->select_all(&temp);

			if (last)
				last->link = curr_by_name;
			else
				curr = curr_by_name;

			last = temp;
		}
	} else if (c_phone == condition::eq && op == operation::land)
	{
		c_phone = condition::none;
		make_validator(val);
		c_phone = condition::eq;

		auto suit_node = db->search_node<record>(*this);
		// Не нашли узла с таким номером
		if (!suit_node)
			return nullptr;

		curr = suit_node->select_valid<command>(*this, val, &last);
	} else if (c_name == condition::eq && op == operation::land)
	{
		c_name = condition::none;
		make_validator(val);
		c_name = condition::eq;

		auto suit_node = db->search_node<record>(*this);
		// Не нашли узла с таким именем
		if (!suit_node)
			return nullptr;

		curr = suit_node->select_valid<command> (*this, val, &last);
	} else
	{
		make_validator(val);
		curr = worm->select_valid(*this, val, &last);
	}

	// Чтобы последний считался за найденный он указывает сам на себя!
	if (last)
		last->link = nullptr;

	return curr;
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

