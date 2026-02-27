#ifndef LIST_H
#define LIST_H

#include "io_status.h"
#include "list_node.h"

#include <cstring>
#include <cstdio>

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

		io_status add_node (list_node<T>& x, list_node<T> **curr)
		{
			auto node = new list_node<T>((list_node<T>&&)x);
			if (node == nullptr)
				return io_status::memory;
			
			if (head == nullptr)
			{
				head = node;
				(*curr) = head;
			} else
			{
				(*curr)->next = node;
				(*curr) = (*curr)->next;
			}

			return io_status::success;
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

		io_status read (char *str, const char *delim)
		{
			char *saveptr = nullptr,
				 *token = nullptr;
			list_node<T> x, *curr = nullptr;
			io_status ret;

			erase();
			token = str;

			while ((token = strtok_r(token, delim, &saveptr)) != nullptr)
			{
				if ((ret = x.read(token)) != io_status::success)
					return ret;

				if ((ret = add_node(x, &curr)) != io_status::success)
				{
					erase();
					return ret;
				}
				
				token = nullptr;
			}

			return io_status::success;
		}

		int with_in (const char *str) const
		{
			for (list_node<T> *curr = head ; curr ; curr = curr->next)
				if (curr->with_in(str, 0, 0))
					return 1;

			return 0;
		}

		int with_percent (const char *str) const
		{
			for (list_node<T> *curr = head ; curr ; curr = curr->next)
				if (curr->with_percent(str, 0, 0))
					return 1;

			return 0;
		}

		int with_underscore (const char *str) const
		{
			for (list_node<T> *curr = head ; curr ; curr = curr->next)
				if (curr->with_underscore(str))
					return 1;

			return 0;
		}

		int fit_one (const char *str) const
		{
			for (list_node<T> *curr = head ; curr ; curr = curr->next)
				if (curr->is_valid(str))
					return 1;

			return 0;
		}

		int fit_all (const char *str) const
		{
			for (list_node<T> *curr = head ; curr ; curr = curr->next)
				if (!curr->is_valid(str))
					return 0;

			return 1;
		}
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
