#ifndef LIST_H
#define LIST_H

#include "io_status.h"
#include "list_node.h"
#include "request.h"

#include <cstdio>
#include <new>

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

		io_status add_node (T&& x)
		{
			list2_node<T> buf = new list2_node<T>;
			if (buf == nullptr)
				return io_status::memory;

			*buf = std::move(x);
			head->prev = buf;
			head = buf;

			return io_status::success;
		}

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

		void print (FILE *fp = stdout) const
		{
			unsigned int i = 0;
			for (const list2_node<T> *curr = head ; (curr && (i < r)) ; curr = curr->next)
			{
				curr->print(fp);
				i++;
			}
		}

		int print_valid (request& x, FILE *fp = stdout) const
		{
			int count = 0;
			for (auto *curr = head ; curr ; curr = curr->next)
				if (x.apply(*curr))
				{
					curr->print(fp, x.order.get());
					++count;
				}

			fprintf(fp, "\n");
			return count;
		}

		int get_length () const
		{
			int len = 0;
			for (list2_node<T> *curr = head ; curr ; len += curr->get_length(), curr = curr->next);
			return len;
		}

		int operator< (const list2 & b) const
		{
			if (!b.head)
				return 0;
			if (!head)
				return 1;
			
			if (*head < *b.head)
				return 1;

			return 0;
		}
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
};

#endif // LIST_H
