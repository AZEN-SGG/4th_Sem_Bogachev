#ifndef DATABASE_H
#define DATABASE_H

#include "io_status.h"
#include "operation.h"
#include "ordering.h"
#include "comparator.h"
#include "search_conditions.h"

#include "list2.h"
#include "list2_node.h"
#include "fast_search.h"
#include "validator.h"

#include <memory>
#include <new>

class command;

template <typename T>
class database
{
private:
	std::unique_ptr<list2<T>> db = nullptr;
	std::unique_ptr<search_structure_store<T>> indexes = nullptr;

public:
	database ()
	{
		db = std::make_unique<list2<T>>();
		indexes = std::make_unique<search_structure_store<T>>();
	}
	~database () = default;

	io_status add (T&& x)
	{
		auto new_node = db->add_node(x);

		if (!new_node)
			return io_status::memory;

		auto ret = indexes->add(new_node);
		if (ret != io_status::success)
			return ret;

		return io_status::success;
	}

	io_status read (FILE *fp, const unsigned int max_read = -1)
	{
		list2_node<T> buf = {},
			*curr = nullptr;

		unsigned int i = 0;
		while ((i < max_read) && (buf.read(fp) == io_status::success))
		{
			curr = new (std::nothrow) list2_node<T>;

			if (!curr)
			{
				erase();
				return io_status::memory;
			}

			*curr = (list2_node<T> &&)buf;
			db->add(curr);

			auto ret = indexes->add(curr);
			if (ret != io_status::success)
				return ret;

			i++;
		} if ((!feof(fp)) && (i < max_read))
		{
			erase();
			return io_status::format;
		}

		return io_status::success;
	}

	// Возвращает количество выведенных элементов
	int print_selected (list2_node<T> *curr, FILE *fp = stdout, const ordering *order = nullptr) { return db->print_sublist(curr, fp, order); }

	void delete_selected (list2_node<T> *curr)
	{
		list2_node<T> *next = nullptr;
		for (; curr ; curr = next)
		{
			indexes->del(curr);

			next = curr->link;

			if (curr->prev)
				curr->prev->next = curr->next;
			else
				db->head = curr->next;

			if (curr->next)
				curr->next->prev = curr->prev;

			delete curr;
		}
	}

	template <typename X>
	list2_node<T> * validate (search_conditions<X>& cond, list2_node<T> **last_selected = nullptr)
	{
		list2_node<T> *origin = nullptr,
						*last = nullptr;
		
		if (
				(cond.op == operation::lor) && 
				(!((cond.c_group == condition::eq || cond.c_group == condition::none) &&
					(cond.c_phone == condition::eq || cond.c_phone == condition::none) &&
					(cond.c_name == condition::eq || cond.c_name == condition::none))
		)) {
			validator<search_conditions<X>, T> val;

			cond.make_validator(val);
			origin = db->select_valid(*this, val, &last);
		} else
			origin = indexes->validate(cond, &last);

		if (last_selected)
			*last_selected = last;
		else if (last)
			last->link = nullptr;

		return origin;
	}
	
	list2_node<T> * sort_selected (list2_node<T> *origin, comparator<T>& comp) { return db->sort(origin); }

	friend class command;

private:
	void erase () { db->erase(); indexes->erase(); }

	list2_node<T> * search_node (const T& x) { return indexes->search_node(x); }
};

#endif // DATABASE_H
