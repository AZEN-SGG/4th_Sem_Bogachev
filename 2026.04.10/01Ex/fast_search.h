#ifndef FAST_SEARCH_H
#define FAST_SEARCH_H

#include "io_status.h"
#include "list2_node.h"
#include "ordering.h"

#include "rb_tree.h"
#include "data_tree.h"
#include "validator.h"

#include <memory>

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

private:
	list2_node<T> * search_node (const T& x)
	{
		auto node = phone_index->search_node(x);
		if (!node)
			return nullptr;

		return node->search_node(x);
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

	io_status add (list2_node<T> *x)
	{
		hash[x->template get_hash<X>()].add(x);
		return io_status::success;
	}

private:
	list2_node<T> * search_node (const T& x) { return hash[x->template get_hash<X>()].search_node(x); }
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

	io_status add (list2_node<T> *x)
	{
		trees->add(x);
		hash->add(x);
	}

private:
	list2_node<T> * search_node (const T& x) { return hash->search_node(x); }
};

#endif // FAST_SEARCH_H
