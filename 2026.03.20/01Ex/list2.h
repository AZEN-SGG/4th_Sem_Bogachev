#ifndef LIST2_H
#define LIST2_H

#include "io_status.h"
#include "list2_node.h"
#include "comparator.h"
#include "ordering.h"
#include "validator.h"

#include <cstdio>
#include <new>

class database;
class command;

template <typename T>
class list2
{
private:
	list2_node<T> * head = nullptr;
	static unsigned int r;
public:
	list2 () = default;
	~list2 () { erase (); }
	
	list2 (const list2 &) = delete;
	list2 (list2 && r)
	{
		head = r.head;
		r.head = nullptr;
	}

	list2 & operator= (const list2 &) = delete;
	list2 & operator= (list2 && r)
	{
		if (this == &r)
			return *this;
		
		erase();

		head = r.head;
		r.head = nullptr;
		
		return *this;
	}

	static void set_r (int r) { list2<T>::r = r; };

	io_status read (FILE *fp = stdin, const unsigned int max_read = -1)
	{
		list2_node<T> buf,
			*prev = nullptr,
			*next = nullptr;
		io_status ret;

		erase();

		if ((ret = buf.read(fp)) != io_status::success)
			return ret;

		head = new list2_node<T>;
		if (head == nullptr)
			return io_status::memory;

		*head = (list2_node<T> &&)buf;
		list2_node<T> *curr = head;

		unsigned int i = 1;
		while ((i < max_read) && (buf.read(fp) == io_status::success))
		{
			next = new list2_node<T>;

			if (!next)
			{
				erase();
				return io_status::memory;
			}

			*next = (list2_node<T> &&)buf;
	
			curr->prev = prev;
			curr->next = next;
	
			prev = curr;
			curr = next;

			i++;
		} if ((!feof(fp)) && (i < max_read))
		{
			erase();
			return io_status::format;
		}

		curr->prev = prev;
		curr->next = nullptr;

		return io_status::success;
	}

	io_status read_file (char *filename, const unsigned int max_read = -1)
	{
		FILE *fp = fopen(filename, "r");
		if (fp == nullptr)
			return io_status::open;

		io_status ret = read(fp, max_read);

		fclose(fp);
		return ret;
	}

	void delete_sublist (list2_node<T> *curr)
	{
		list2_node<T> *next = nullptr;
		for (; curr ; curr = next)
		{
			next = curr->link;

			if (curr->prev)
				curr->prev->next = curr->next;
			else
				head = curr->next;

			if (curr->next)
				curr->next->prev = curr->prev;

			delete curr;
		}
	}

	void print (FILE *fp = stdout) const
	{
		unsigned int i = 0;
		for (const list2_node<T> *curr = head ; (curr && (i < r)) ; curr = curr->next)
		{
			curr->print(fp);
			i++;
		}
	}

	static int print_sublist (const list2_node<T> *curr, FILE *fp = stdout, const ordering *order = nullptr)
	{
		const ordering default_ordering[ORDERING_LEN] = {ordering::name, ordering::phone, ordering::group};
		if (!order)
			order = default_ordering;

		int i = 0;
		for (; curr ; curr = curr->link, ++i)
			curr->print(fp, order);

		return i;
	}

	template <typename X>
	list2_node<T> * select_valid (const X& x, validator<X, T> &val) const
	{
		list2_node<T> *origin = nullptr, *prev = nullptr;
		for (auto *curr = head ; curr ; curr = curr->next)
			if (val(x, *curr))
			{
				if (prev)
					prev->link = curr;
				else
					origin = curr;

				prev = curr;
			}

		if (prev)
			prev->link = nullptr;

		return origin;
	}

	// Adding sorted list B to sorted list A
	list2_node<T> * add_sorted_to_sorted (list2_node<T> *l_temp, int len_add, list2_node<T> **end_A, comparator<T>& comp)
	{
		list2_node<T> *added = nullptr,
				  *added_next = nullptr,
				  *start = head, // Start for adding into list
				  *prev_start = nullptr;

		int i;
		for (added = l_temp, i = 0 ; ((added != nullptr) && (i < len_add)) ; added = added_next, ++i)
		{
			list2_node<T> *curr = nullptr,
					  *prev = nullptr;

			added_next = added->link;

			for (prev = prev_start, curr = start ; (curr != nullptr) && (comp(*curr, *added) < 0) ; prev = curr, curr = curr->link);
			if (curr == nullptr)
				*end_A = added;

			if (prev == nullptr)
				head = added;
			else
				prev->link = added;
			
			added->link = curr;
			
			start = curr;
			prev_start = added;
		}

		return added;
	}

	static list2_node<T> * sort (list2_node<T> *origin, comparator<T>& comp)
	{
		int k = 0,
			n = 1;

		if (!origin)
			return nullptr;

		// Main loop
		while (k != 1)
		{
			k = 0;

			list2_node<T> *start = nullptr,
					  *sorted_end = nullptr;
			list2 sorted;

			for (start = origin ; (start != nullptr) ; k++)
			{
				list2 A;
				list2_node<T> *end_A = nullptr,
						  *start_B = nullptr;

				// Searching end of A
				int i;
				for (i = 0, end_A = start ; (end_A != nullptr) && (i < (n - 1)) ; ++i, end_A = end_A->link);
				
				// Length of A less than n
				if (end_A == nullptr)
				{
					if (sorted_end == nullptr)
						sorted.head = origin;
					else
						sorted_end->link = start;

					start = nullptr;
					// Can add sorted_end = nullptr, but it is not necessity
					continue;
				}
		
				// Init A
				A.head = start;
				start_B = end_A->link;
				end_A->link = nullptr; // End of the list must be nullptr
		
				// Adding B into A
				start = A.add_sorted_to_sorted(start_B, n, &end_A, comp);
				
				if (sorted_end == nullptr)
					sorted.head = A.head;
				else
					sorted_end->link = A.head;
				
				sorted_end = end_A;

				A.head = nullptr;
			}

			origin = sorted.head;
			sorted.head = nullptr;

			n <<= 1;
		}

		return origin;
	}

	friend class database;
	friend class command;
private:
	void erase ()
	{
		list2_node<T> *next = nullptr;
		for (list2_node<T> *curr = head ; curr ; curr = next)
		{
			next = curr->next;
			delete curr;
		}

		head = nullptr;
	}

	void add (list2_node<T> *curr)
	{
		if (head)
		{
			head->prev = curr;
			curr->next = head;
		}

		head = curr;
	}

	// Подразумевается, что такого нет!
	list2_node<T> * add_node (T&& x)
	{
		auto buf = new list2_node<T>;
		if (buf == nullptr)
			return nullptr;

		static_cast<T&>(*buf) = static_cast<T&&>(x);
		
		if (head)
			head->prev = buf;
		
		buf->next = head;
		head = buf;

		return io_status::success;
	}
};

#endif // LIST2_H
