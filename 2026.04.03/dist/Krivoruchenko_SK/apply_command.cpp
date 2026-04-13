#include "command.h"
#include "condition.h"
#include "list2.h"
#include "list2_node.h"
#include "operation.h"
#include "rb_tree.h"
#include "record.h"


int command::apply (list2<record> *worm, name_index_t *name_index, phone_index_t *phone_index)
{
	int res = 0;

	switch (type)
	{
		case command_type::insert:
			apply_insert(worm, name_index, phone_index);
			break;
		case command_type::select:
			res = apply_select(worm, name_index, phone_index);
			break;
		case command_type::del:
			apply_delete(worm, name_index, phone_index);
			break;
		case command_type::quit:
			break;
		case command_type::none:
			break;
	}

	return res;
}

// Бинарный поиск одинакового элемента при добавлении
void command::apply_insert (list2<record> *worm, name_index_t *name_index, phone_index_t *phone_index)
{
	// Ищем по телефону, так как поиск по нему быстрее
	auto phone_curr = phone_index->search_node(static_cast<const record&>(*this));

	// В дереве есть список с таким же именем, ищем элемент в этом списке
	if (phone_curr)
	{
		// Если нашёл такой элемент, то добавлять его не нужно
		if (phone_curr->search_node(*this))
			return;
	}
	
	auto new_node = worm->add_node(static_cast<record&&>(*this));

	if (!new_node)
		return;

	if (phone_curr)
		phone_curr->add(new_node);
	else
	{
		auto phone_leaf = phone_index->add(new_node);
		phone_index->fix_tree(phone_leaf);
	}

	auto name_curr = name_index->search_node(static_cast<const record&>(*this));
	if (name_curr)
		name_curr->add(new_node);
	else
	{
		auto name_leaf = name_index->add(new_node);
		name_index->fix_tree(name_leaf);
	}
}

int command::apply_select (list2<record> *worm, name_index_t *name_index, phone_index_t *phone_index)
{
	// Получили head подсписка
	auto curr = validate(worm, name_index, phone_index);

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

void command::apply_delete (list2<record> *worm, name_index_t *name_index, phone_index_t *phone_index)
{
	list2_node<record> 	*curr = validate(worm, name_index, phone_index),
						*next = nullptr;
	for (; curr ; curr = next)
	{
		name_index->del(curr);
		phone_index->del(curr);

		next = curr->link;

		if (curr->prev)
			curr->prev->next = curr->next;
		else
			worm->head = curr->next;

		if (curr->next)
			curr->next->prev = curr->prev;

		delete curr;
	}
}

list2_node<record> * command::validate (list2<record> *worm, name_index_t *name_index, phone_index_t *phone_index)
{
	list2_node<record> 	*curr = nullptr,
						*last = nullptr;

	validator<command, record> val;
	if (c_name == condition::eq
		&& c_phone == condition::eq
		&& c_group == condition::none
		&& op == operation::lor 
	) {
		auto suit_phone = phone_index->search_node<record>(*this);
		// Нашли узел с таким телефоном
		if (suit_phone)
			curr = suit_phone->select_all(&last);

		auto suit_name = name_index->search_node<record>(*this);
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

		auto suit_node = phone_index->search_node<record>(*this);
		// Не нашли узла с таким номером
		if (!suit_node)
			return nullptr;

		curr = suit_node->select_valid<command>(*this, val, &last);
	} else if (c_name == condition::eq && op == operation::land)
	{
		c_name = condition::none;
		make_validator(val);
		c_name = condition::eq;

		auto suit_node = name_index->search_node<record>(*this);
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

