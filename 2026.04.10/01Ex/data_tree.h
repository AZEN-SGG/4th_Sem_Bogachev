#ifndef DATA_TREE_H
#define DATA_TREE_H

#include "io_status.h"
#include "ordering.h"
#include "list.h"
#include "list2_node.h"
#include "list_node.h"
#include "rb_tree.h"
#include "validator.h"
#include "search_conditions.h"

#include <memory>
#include <utility>

template <typename T, ordering X>
class data_tree;

template <typename T>
class data_list
{
private:
	list2_node<T> *node = nullptr;

public:
	data_list () = default;
	~data_list () = default;

	data_list (const data_list&) = delete;
	data_list operator= (const data_list&) = delete;

	bool is_equal (const T& x) { return node->is_equal(x); }

	void add (list2_node<T> *x) { node = x; }

	template <typename U, ordering X>
	friend class data_tree;
};


// X - параметр сравнения - он задаёт через какое поле идёт сравнение
template <typename T, ordering X>
class data_tree
{
private:
	std::unique_ptr<list<data_list<T>>> uniform = nullptr;
	list2_node<T> *node = nullptr;
public:
	data_tree () = default;
	~data_tree () = default;

	data_tree& operator= (const data_tree&) = delete;
	data_tree& operator= (data_tree&& x)
	{
		if (this != &x)
		{
			uniform = std::move(x.uniform);

			node = x.node;
			x.node = nullptr;
		}

		return *this;
	}

	void print (FILE *fp = stdout ) const { node->print(fp); }

	// Подразумевается, что он не может быть без элемента!
	int cmp (const T& x) const { return node->template cmp<X>(x); }
	int cmp (const data_tree<T, X>& x) const { return node->template cmp<X>(x.node); }

	io_status add (list2_node<T> *x)
	{
		io_status ret = io_status::success;

		if (!node)
			node = x;
		else
		{
			if (!uniform)
			{
				uniform = std::make_unique<list<data_list<T>>>();

				if (!uniform)
					return io_status::memory;

				if ((ret = uniform->add(node)) != io_status::success)
					return ret;
			}

			ret = uniform->add(x);
		}

		return ret;
	}

	list2_node<T> * search_node (const T& x)
	{
		if (uniform)
		{
			auto suit_node = uniform->search_node(x);
			
			if (suit_node)
				return suit_node->node;
		} else if (node->is_equal(x))
			return node;

		return nullptr;
	}

	template <typename U>
	list2_node<T> * select_valid (const search_conditions<U>& x, validator<search_conditions<U>, T>& val, list2_node<T> **last = nullptr) const
	{
		// Выбор идёт последовательно - это важно!
		if (uniform)
		{
			list2_node<T> 	*origin = nullptr,
							*prev 	= nullptr;

			list_node<data_list<T>> *curr = uniform->head;
			for (; curr ; curr = curr->next)
				if (val(x, *(curr->node)))
				{
					// Если нет следующего, значит его ещё не находили 
					if (!curr->node->link)
					{
						if (prev)
						{
							prev->link = curr->node;
							prev = curr->node;
						} else
							origin = prev = curr->node;
					}
				}

			if (prev)
				prev->link = prev;

			if (last)
				*last = prev;

			return origin;
		} else
			if (val(x, *node))
			{
				node->link = node;
				if (last)
					*last = node;

				return node;
			}

		// Ничего не нашли
		return nullptr;
	}

	list2_node<T> * select_all (list2_node<T> **last = nullptr) const
	{
		// Выбор идёт последовательно - это важно!
		if (uniform)
		{
			list2_node<T> 	*origin = nullptr,
							*prev 	= nullptr;

			list_node<data_list<T>> *curr = uniform->head;
			for (; curr ; curr = curr->next)
				if (!curr->node->link)
				{
					if (prev)
					{
						prev->link = curr->node;
						prev = curr->node;
					} else
						origin = prev = curr->node;
				}

			if (prev)
				prev->link = prev;

			if (last)
				*last = prev;

			return origin;
		} else
		{
			node->link = node;
			if (last)
				*last = node;

			return node;
		}

		// Ничего не нашли
		return nullptr;
	}

	bool del (list2_node<T> *x)
	{
		if (uniform)
		{
			list_node<data_list<T>> *curr = uniform->head,
									*prev = nullptr;
			for (; curr ; curr = curr->next)
			{
				if (curr->node == x)
				{
					if (prev)
						prev->next = curr->next;
					else
						uniform->head = curr->next;

					delete curr;

					if (!uniform->head)
						return true;
					
					node = uniform->head->node;
					break;
				} else
					prev = curr;
			}
		} else if (node == x)
			return true;

		return false;
	}

	friend class rb_tree<T>;
private:
	void erase () { uniform = nullptr; node = nullptr; }
};

#endif // DATA_TREE_H
