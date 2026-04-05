#ifndef DATABASE_H
#define DATABASE_H

#include "io_status.h"
#include "ordering.h"

#include "list2.h"
#include "list2_node.h"
#include "fast_search.h"

#include <memory>
#include <new>

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

	void del (list2_node<T> *curr)
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

private:
	void erase () { db->erase(); indexes->erase(); }
};

#endif // DATABASE_H
