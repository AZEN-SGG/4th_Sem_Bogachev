#ifndef B_TREE_H
#define B_TREE_H

#include "io_status.h"
#include "b_tree_node.h"

#include <cstring>
#include <cstdio>
#include <new>


template <typename T>
class b_tree
{
	private:
		int m = 0;
		b_tree_node<T> *root = nullptr;
	
	public:
		b_tree (int base = 0) { m = base; }
		~b_tree ()
		{
			delete_subtree(root);
			erase_links();
		}

		io_status read (char *str, const char *delim)
		{
			char *saveptr = nullptr;
			T x;

			if ((str = strtok_r(str, delim, &saveptr)) == nullptr) // saveptr сохраняет положение первого символа после пробельного
				return io_status::success;

			do {
				x.read(str);
				io_status ret = add_value(x);
				if (ret != io_status::success)
					return ret;
			} while ((str = strtok_r(nullptr, delim, &saveptr)) != nullptr); // Через nullptr функция понимает, что итерация не 1я

			return io_status::success;
		}

		io_status read (FILE *fp = stdin, unsigned int max_read = -1)
		{
			unsigned int readed = 0;
			io_status ret;
			T x;

			while ((readed < max_read) && (x.read(fp) == io_status::success))
			{
				if ((ret = add_value(x)) != io_status::success)
				{
					delete_subtree(root);
					erase_links();

					return io_status::memory;
				}
				readed++;
			} if ((!feof(fp)) && (readed < max_read))
			{
				delete_subtree(root);
				erase_links();

				return io_status::format;
			}

			return io_status::success;
		}

		io_status read_file (char *filename, unsigned int max_read = -1)
		{
			FILE *fp = fopen(filename, "r");
			if (!fp)
				return io_status::open;
		
			io_status ret = read(fp, max_read);
		
			fclose(fp);
			return ret;
		}
		
		void print (int r, FILE *fp = stdout) const
		{
			print_subtree (root, 0, r, fp);
		}

		b_tree_node<T> * find (const char *x) const
		{
			int index, cmp;
			b_tree_node<T> *curr = root;
			while (curr != nullptr)
			{
				index = curr->bin_search(x, &cmp);

				if (cmp == 0)
					break;
				
				curr = curr->children[index];
			}
			
			return curr;
		}

//		friend io_status solve_01 (char * f_in, char * f_out, char * s, char * t, int m, int * r);
	private:
		void erase_links ()
		{
			m = 0;
			root = nullptr;
		}

		static void delete_subtree (b_tree_node<T> *curr)
		{
			if (curr == nullptr)
				return;
			for (int i = 0 ; i <= curr->size ; ++i)
				delete_subtree(curr->children[i]);

			delete curr;
		}
		
		static void print_subtree (const b_tree_node<T> *curr, int level, int r, FILE *fp = stdout)
		{
			if (curr == nullptr || level > r)
				return;

			curr->print(level, fp);
			for (int i = 0 ; i <= curr->size ; ++i)
			{
				if (curr->children[i] && level + 1 <= r)
				{
					for (int j = 0 ; j < level ; ++j)
						fprintf(fp, "  ");
					fprintf(fp, "children[%2d]\n", i);
					print_subtree(curr->children[i], level + 1, r, fp);
				}
			}
		}
		
		io_status add_value (T &x)
		{
			io_status ret = io_status::success;

			if (root == nullptr)
			{
				root = new b_tree_node<T>();
				if (root == nullptr)
					return io_status::memory;

				if ((ret = root->init(m)) != io_status::success)
				{
					delete root;
					return ret;
				}

				root->values[0] = static_cast<T&&>(x);
				root->size = 1;
				return ret;
			}

			b_tree_node<T> *curr = root,
				*down = nullptr;
			ret = add_value_subtree(curr, down, x, m);

			if (ret != io_status::create)
				return ret;

			b_tree_node<T> *p = new b_tree_node<T>();
			if (p == nullptr)
				return io_status::memory;

			if ((ret = p->init(m)) != io_status::success)
			{
				delete p;
				return ret;
			}

			p->values[0] = static_cast<T&&>(x);
			p->children[0] = curr;
			p->children[1] = down;
			p->size = 1;

			root = p;

			return ret;
		}

		static io_status add_value_subtree (b_tree_node<T> *&curr, b_tree_node<T> *&down, T &x, int m)
		{
			int index = curr->bin_search(x);
			b_tree_node<T> *p = curr->children[index];

			if (p != nullptr)
			{
				io_status ret = add_value_subtree(p, down, x, m);
				if (ret != io_status::create)
					return ret;
			} else
				down = nullptr;

			if (curr->size < m * 2)
			{
				curr->add_value(x, down, index);
				return io_status::success;
			} else
			{
				b_tree_node<T> *p = new b_tree_node<T>();
				if (p == nullptr)
					return io_status::memory;

				io_status ret;
				if ((ret = p->init(m)) != io_status::success)
				{
					delete p;
					return ret;
				}
				
				if (index == m)
				{
					for (int i = 1 ; i <= m ; ++i)
					{
						p->values[i - 1] = static_cast<T&&>(curr->values[i + m - 1]);
						p->children[i] = curr->children[i + m];
						curr->children[i + m] = nullptr;
					}

					p->children[0] = down;
				} else if (index < m)
				{
					for (int i = 0 ; i < m ; ++i)
					{
						p->values[i] = static_cast<T&&>(curr->values[i + m]);
						p->children[i] = curr->children[i + m];
						curr->children[i + m] = nullptr;
					}

					p->children[m] = curr->children[m * 2];
					curr->children[m * 2] = nullptr;

					for (int i = m ; i > index ; --i)
					{
						curr->values[i] = static_cast<T&&>(curr->values[i - 1]);
						curr->children[i + 1] = curr->children[i];
					}

					curr->values[index] = static_cast<T&&>(x);
					curr->children[index + 1] = down;
					
					x = static_cast<T&&>(curr->values[m]);
				} else if (index > m)
				{
					p->children[0] = curr->children[m + 1];
					curr->children[m + 1] = nullptr;

					for (int i = 1 ; i < index - m ; ++i)
					{
						p->values[i - 1] = static_cast<T&&>(curr->values[m + i]);
						p->children[i] = curr->children[m + i + 1];
						curr->children[m + i + 1] = nullptr;
					}

					p->values[index - m - 1] = static_cast<T&&>(x);
					p->children[index - m] = down;

					for (int i = index - m + 1 ; i <= m ; ++i)
					{
						p->values[i - 1] = static_cast<T&&>(curr->values[m + i - 1]);
						p->children[i] = curr->children[m + i];
						curr->children[m + i] = nullptr;
					}

					x = static_cast<T&&>(curr->values[m]);
				}

				down = p;
				p->size = m;
				curr->size = m;
				
				return io_status::create;
			}

			return io_status::success;
		}
};

#endif // B_TREE_H
