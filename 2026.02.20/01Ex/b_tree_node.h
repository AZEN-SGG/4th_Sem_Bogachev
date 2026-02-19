#ifndef B_TREE_NODE_H
#define B_TREE_NODE_H

#include "io_status.h"

#include <cstdio>
#include <new>

template <typename T> class b_tree;

template <typename T>
class b_tree_node
{
	private:
		T *values = nullptr;
		int size = 0;
		b_tree_node **children = nullptr;
	
	public:
		b_tree_node () = default;
		~b_tree_node () { delete_node(); }

		b_tree_node (const b_tree_node &x) = delete;
		b_tree_node (b_tree_node &&x)
		{
			values = x.values;
			children = x.children;
			size = x.size;

			x.erase_links();
		}

		b_tree_node & operator= (const b_tree_node &x) = delete;
		b_tree_node & operator= (b_tree_node &&x)
		{
			if (this == &x)
				return *this;
			delete_node();

			values = x.values;
			children = x.children;
			size = x.size;

			x.erase_links();
			return *this;
		}
		
		void print (int p = 0, FILE *fp = stdout) const
		{
			for (int i = 0 ; i < size ; ++i)
			{
				for (int j = 0 ; j < p ; j++)
					fprintf(fp, "  ");
				fprintf(fp, "values[%2d] ", i + 1);
				values[i].print(fp);
			}
		}

		int bin_search (const T &x) const
		{
			int l = 0,
				r = size,
				m;

			while (l != r)
			{
				m = (l + r) / 2;
				if (values[m] < x)
					l = m + 1;
				else
					r = m;
			}

			return l;
		}
		
		// For unique_ptr<char[]>
		int bin_search (const char *x, int *cmp) const
		{
			int l = 0,
				r = size,
				m;

			while (l != r)
			{
				m = (l + r) / 2;

				*cmp = values[m].cmp(x);
				if (*cmp < 0)
					l = m + 1;
				else
					r = m;
			}

			return l;
		}

		friend class b_tree<T>;
	
	private:
		void erase_links ()
		{
			values = nullptr;
			children = nullptr;
			size = 0;
		}

		void delete_node ()
		{
			if (values != nullptr)
				delete[] values;

			if (children != nullptr)
				delete[] children;

			erase_links();
		}

		io_status init (int m)
		{
			values = new T[m * 2];
			if (values == nullptr)
				return io_status::memory;

			children = new b_tree_node*[m * 2 + 1];
			if (children == nullptr)
			{
				delete[] values;
				values = nullptr;
				return io_status::memory;
			}

			for (int i = 0 ; i < m * 2 + 1 ; ++i)
				children[i] = nullptr;

			size = 0;
			return io_status::success;
		}
		
		void add_value (T &x, b_tree_node<T> *down, int index)
		{
			for (int i = size ; i > index ; --i)
			{
				values[i] = static_cast<T&&>(values[i - 1]);
				children[i + 1] = children[i];
			}

			values[index] = static_cast<T&&>(x);
			children[index + 1] = down;
			size++;
		}
};

#endif // B_TREE_NODE_H
