#ifndef FAST_SEARCH_H
#define FAST_SEARCH_H

#include "condition.h"
#include "io_status.h"
#include "list2_node.h"
#include "operation.h"
#include "ordering.h"
#include "rb_tree_node.h"
#include "search_conditions.h"

#include "rb_tree.h"
#include "data_tree.h"
#include "validator.h"

#include <memory>

template <typename T>
class database;

template <typename T>
class search_structure_store;

template <typename T, ordering X>
class index_array;

template <typename T>
class index_trees
{
public:
	using name_index_t = rb_tree<data_tree<T, ordering::name>>;
	using phone_index_t = rb_tree<data_tree<T, ordering::phone>>;

private:
	std::unique_ptr<name_index_t> name_index = nullptr;
	std::unique_ptr<phone_index_t> phone_index = nullptr;

public:
	index_trees ()
	{
		name_index = std::make_unique<name_index_t>();
		phone_index = std::make_unique<phone_index_t>();
	}

	template <typename U, ordering X>
	friend class index_array;
	friend class search_structure_store<T>;

private:
	void erase () { name_index->erase(); phone_index->erase(); }
	list2_node<T> * search_node (const T& x)
	{ 
		auto node =  phone_index->search_node(x);
		if (!node)
			return nullptr;

		return node->search_node(x);
	}

	io_status add (list2_node<T> *x)
	{
		auto name_node = name_index->add(x);
		if (!name_node)
			return io_status::memory;
		name_index->fix_tree(name_node);

		auto phone_node = phone_index->add(x);
		if (!phone_node)
			return io_status::memory;
		phone_index->fix_tree(phone_node);

		return io_status::success;
	}

	void del (list2_node<T> *x)
	{
		name_index->del(x);
		phone_index->del(x);
	}

	template <typename X>
	list2_node<T> * validate (search_conditions<X>& x, list2_node<T> **last_selected = nullptr)
	{
		list2_node<T> *origin 	= nullptr,
				   *last	= nullptr;

		if (x.op == operation::lor)
		{
			if (x.c_name == condition::eq)
				origin = select_all(name_index, &last);

			if (x.c_phone == condition::eq)
			{
				list2_node<T> *temp = nullptr,
								*temp_l = nullptr;

				temp = select_all(phone_index, &temp_l);
				
				if (last)
					last->link = temp;
				else
					origin = temp;

				last = temp_l;
			}
		} else
		{
			validator<X, T> val;

			if (x.c_name == condition::eq)
			{
				x.c_name = condition::none;
				make_validator(val);
				x.c_name = condition::eq;

				auto suit_node = name_index->template search_node<T>(x);
				// Не нашли узла с таким именем
				if (!suit_node)
				{
					origin = nullptr;
					last = nullptr;
				} else
					origin = suit_node->template select_valid<X>(x, val, &last);
			} else if (x.c_phone == condition::eq)
			{
				x.c_phone = condition::none;
				make_validator(val);
				x.c_phone = condition::eq;

				auto suit_node = phone_index->template search_node<T>(x);
				// Не нашли узла с таким номером
				if (!suit_node)
				{
					origin = nullptr;
					last = nullptr;
				} else
					origin = suit_node->template select_valid<X>(x, val, &last);
			} else
				origin = select_all(&last);
		}

		if (last_selected)
			*last_selected = last;
		else if (last)
			last->link = nullptr;

		return origin;
	}

	list2_node<T> select_all ()
	{
		rb_tree_node<data_tree<T, ordering::phone>> *curr = phone_index->root,
													*last = nullptr;
		for (; curr ; curr = curr->)
	}

	static list2_node<T> select_all_in_subtree (
			rb_tree_node<data_tree<T, ordering::phone>> *curr,
			list2_node<T> **last
	) {
		list2_node<T> *temp = nullptr;
		if (curr)
		{
			curr->select_all(&temp);
			(*last) = temp;

			select_all_in_subtree(curr->left);
		}

		for (; curr ; curr = curr->right)
		{
			last->link = curr->select_all(&temp);
			last = temp;
		}
	}
};

template <typename T, ordering X>
class index_array
{
public:
	static const int max_size;

private:
	std::unique_ptr<index_trees<T> []> hash = nullptr;

public:
	index_array () { hash = std::make_unique<index_trees<T> []>(max_size); }

	index_array (const index_array& x) = delete;
	index_array & operator= (const index_array& x) = delete;

	friend class search_structure_store<T>;

private:
	void erase () { for (int i = 0 ; i < max_size ; ++i) hash[i]->erase(); }
	list2_node<T> * search_node (const T& x) { return hash[x->template get_hash<X>()].search_node(x); }

	io_status add (list2_node<T> *x)
	{
		hash[x->template get_hash<X>()].add(x);
		return io_status::success;
	}

	void del (list2_node<T> *x) { hash[static_cast<T*>(x->template get_hash<X>())].del(x); }

	template <typename U>
	list2_node<T> * validate (search_conditions<U>& x, list2_node<T> **last)
	{
		list2_node<T> *origin = nullptr;

		if (x.op == operation::lor)
			origin = hash[x->template get_hash<X>()]->select_all(last);
		else
			origin = hash[x->template get_hash<X>()]->validate(x, last);

		return origin;
	}
};

template <typename T>
class search_structure_store
{
private:
	std::unique_ptr<index_trees<T>> trees = nullptr;
	std::unique_ptr<index_array<T, ordering::group>> hash = nullptr;

public:
	search_structure_store ()
	{
		trees = std::make_unique<index_trees<T>>();
		hash = std::make_unique<index_array<T, ordering::group>>();
	}

	search_structure_store (const search_structure_store& x) = delete;
	search_structure_store & operator= (const search_structure_store& x) = delete;

	friend class database<T>;

private:
	void erase () { trees->erase(); hash->erase(); }
	list2_node<T> * search_node (const T& x) { return hash->search_node(x); }

	// Приватная, так как эти классы обязательно содержатся в базе, они не существуют отдельно!
	io_status add (list2_node<T> *x)
	{
		trees->add(x);
		hash->add(x);
	}

	void del (list2_node<T> *curr)
	{
		trees->del(curr);
		hash->del(curr);
	}

	template <typename X>
	list2_node<T> * validate (search_conditions<X>& x, list2_node<T> **last_selected = nullptr)
	{
		list2_node<T> *origin = nullptr,
						*last = nullptr;

		if (x.op == operation::lor)
		{
			if (x.c_group == condition::eq)
				origin = hash->validate(x, &last);

			if (x.c_name == condition::eq ||
				x.c_phone == condition::eq)
			{
				list2_node<T> *temp = nullptr,
								*temp_l = nullptr;
				temp = trees->validate(x, &temp_l);

				if (last != nullptr)
					last->link = temp;
				else
					origin = temp;

				last = temp_l;
			}
		} else
		{
			if (x.c_group == condition::eq)
				origin = hash->validate(x, &last);
			else
				origin = trees->validate(x, &last);
		}

		*last_selected = last;
		return origin;
	}
};

#endif // FAST_SEARCH_H
