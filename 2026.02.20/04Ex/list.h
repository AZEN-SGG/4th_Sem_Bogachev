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

        void print (unsigned int r, FILE *fp = stdout) const
		{
			unsigned int i = 0;
			for (const list_node<T> *curr = head; ((curr != nullptr) && (i < r)); curr = curr->next)
			{
				curr->print(fp);
				i++;
			}
		}

		io_status read (char *str, char *oper, const char *delim)
		{
			char *savekey = nullptr,
				 *savevalue = nullptr,
				 *key = nullptr,
				 *value = nullptr;
			list_node<T> x, *curr;

			if ((key = strtok_r(str, delim, &savekey)) != nullptr)
			{
				if ((value = strtok_r(oper, delim, &savevalue)) == nullptr)
					return io_status::format;

				io_status ret = x.read(key, value);
				if (ret != io_status::success)
					return ret;

				auto node = new list_node<T>((list_node<T>&&)x);
				if (node == nullptr)
					return io_status::memory;
				head = node;
			}

			curr = head;

			while ((key = strtok_r(nullptr, delim, &savekey)) != nullptr)
			{
				if ((value = strtok_r(nullptr, delim, &savevalue)) == nullptr)
					return io_status::format;
			
				io_status ret = x.read(key, value);
				if (ret != io_status::success)
					return ret;

				auto node = new list_node<T>((list_node<T>&&)x);
				if (node == nullptr)
				{
					erase();
					return io_status::memory;
				}
				
				curr->next = node;
				curr = node;
			}

			return io_status::success;
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

		int fit_one (const char *str) const
		{
			for (list_node<T> *curr = head ; curr ; curr = curr->next)
				if (curr->is_valid(str))
					return 1;

			return 0;
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
