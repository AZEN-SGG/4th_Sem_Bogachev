#ifndef DATA_TREE_H
#define DATA_TREE_H

#include "io_status.h"
#include "list.h"
#include "list2_node.h"
#include "rb_tree.h"

template <typename T>
class data_list
{
private:
	list2_node<T> *node = nullptr;
};

template <typename T>
class data_tree
{
private:
	list<data_list<T>> *list = nullptr;
	list2_node<T> *node = nullptr;
public:
	data_tree () = default;
	~data_tree () = default;

	int cmp (char *str) { return node->cmp(str); }

	io_status add (list2_node<T> *x)
	{
		io_status ret = io_status::success;

		if (!node)
			node = x;
		else
			ret = list->add(x);

		return ret;
	}

	friend class rb_tree<T>;
private:
	erase () { list = nullptr; node = nullptr; }
};

#endif // DATA_TREE_H
