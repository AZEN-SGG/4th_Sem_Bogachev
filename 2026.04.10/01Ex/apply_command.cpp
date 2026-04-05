#include "command.h"
#include "condition.h"
#include "list2.h"
#include "list2_node.h"
#include "operation.h"
#include "rb_tree.h"
#include "record.h"


int command::apply (database<record> *db)
{
	int res = 0;

	switch (type)
	{
		case command_type::insert:
			apply_insert(worm, db);
			break;
		case command_type::select:
			res = apply_select(worm, db);
			break;
		case command_type::del:
			apply_delete(worm, db);
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
	auto curr = validate(worm, db);

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
	auto curr = validate(db);
	db->del(curr);
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
		auto suit_phone = db->search_node<record>(*this);
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

void command::make_validator (validator<command, record>& val) const
{
	val.op = op;

	validator<command, record>::valid_t neutral = nullptr;
	switch (op)
	{
		case operation::land:
			neutral = &command::is_true;
			break;
		case operation::lor:
			neutral = &command::is_false;
			break;
		case operation::none:
			neutral = nullptr;
			break;
	}

	if (c_group != condition::none)
		val.fgroup = &command::cmp_group;
	else
		val.fgroup = neutral;

	if (c_phone != condition::none)
		val.fphone = &command::cmp_phone;
	else
		val.fphone = neutral;

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
bool command::cmp_phone (record& x) const { return x.compare_phone(c_phone, *this); }
bool command::cmp_name (record& x) const { return x.compare_word(c_name, *this); }
bool command::like_name (record& x) const { return pattern::is_valid(x.get_word(), 0, 0); }
bool command::nlike_name (record& x) const { return !pattern::is_valid(x.get_word(), 0, 0); }

