#ifndef LIST_H
#define LIST_H

#include "io_status.h"
#include "list_node.h"
#include "list2_node.h"
#include "validator.h"

#include <cstdio>
#include <new>

template <typename X, typename U>
class data_tree;

template <typename T>
class list
{
private:
	list_node<T> *head = nullptr;
public:
	list () = default;
	~list () { erase(); }

	list& operator= (const list&) = delete;
	list& operator= (list&& r)
	{
		if (this->head == r.head)
			return *this;

		erase();

		head = r.head;
		r.head = nullptr;

		return *this;
	}

	void print (unsigned int r, FILE *fp = stdout) const
	{
		unsigned int i = 0;
		for (const list_node<T> *curr = head; ((curr != nullptr) && (i < r)); curr = curr->next)
		{
			curr->print(fp);
			i++;
		}
	}

	io_status read (FILE *fp = stdin, unsigned int max_read = -1)
	{
		list_node<T> buf, *tail;
		unsigned int i = 0;
		io_status ret;

		if ((ret = buf.read(fp)) != io_status::success)
			return ret;

		head = new list_node<T>;
		if (head == nullptr)
			return io_status::memory;

		*head = (list_node<T>&&)buf;
		list_node<T> *curr = head;

		while ((i < max_read) && (buf.read(fp) == io_status::success))
		{
			tail = new list_node<T>;

			if (tail == nullptr)
			{
				erase();
				return io_status::memory;
			}

			*tail = (list_node<T>&&)buf;
			curr->next = tail;
			curr = tail;
			
			i++;
		} if ((i < max_read) && (!feof(fp)))
		{
			erase();
			return io_status::format;
		}

		return io_status::success;
	}

	io_status read_file (const char *filename)
	{
		FILE *fp = fopen(filename, "r");

		if (fp == nullptr)
			return io_status::open;
		
		io_status ret = read(fp);
		
		fclose(fp);
		return ret;	
	}

	template <typename X>
	io_status add (const X *x)
	{
		auto curr = new (std::nothrow) list_node<T>();
		if (!curr)
			return io_status::memory;

		curr->add(x);

		curr->next = head;
		head = curr;
	}

	template <typename X>
	list_node<T> * search_node (const X& x)
	{
		auto curr = head;
		for (; curr ; curr = curr->next)
			if (curr->is_equal(x))
				return curr;

		return nullptr;
	}

	template <typename X, typename U>
	friend class data_tree;
private:
	void erase ()
	{
		list_node<T> *next;
		for (list_node<T> *curr = head; curr; curr = next)
		{
			next = curr->next;
			delete curr;
		}
		
		head = nullptr;
	}
};

#endif // LIST_H
