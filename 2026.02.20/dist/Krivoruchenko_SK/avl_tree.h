#ifndef AVL_TREE_H
#define AVL_TREE_H

#include "io_status.h"
#include "avl_tree_node.h"

#include <cstring>

template <typename T>
class avl_tree
{
	private:
		avl_tree_node<T> *root = nullptr;
	
	public:
		avl_tree () = default;
		~avl_tree ()
		{
			delete_subtree(root);
			root = nullptr;
		}

		io_status read (char *str, char *replace, const char *delim)
		{
			char *savekey = nullptr,
				 *savevalue = nullptr,
				 *key = nullptr,
				 *value = nullptr;
			avl_tree_node<T> x;

			while ((key = strtok_r(str, delim, &savekey)) != nullptr)
			{
				value = strtok_r(replace, delim, &savevalue);

				str = replace = nullptr;

				x.read(key, value);
				auto node = new avl_tree_node<T>((avl_tree_node<T>&&)x);
				if (node == nullptr)
				{
					delete_subtree(root);
					return io_status::memory;
				}

				add_node(node);
			}

			return io_status::success;
		}
	
		io_status read (FILE *fp, const unsigned int max_read = -1)
		{
			avl_tree_node<T> x;
			io_status ret;

			unsigned int readed = 0;
			while ((x.read(fp) == io_status::success) && (readed < max_read))
			{
				avl_tree_node<T> *node = new avl_tree_node<T>((avl_tree_node<T>&&)x);
				if (!node)
				{
					delete_subtree(root);
					root = nullptr;
					return io_status::memory;
				}

				add_node(node);
				readed++;
			} if ((!feof(fp)) && (readed < max_read))
			{
				delete_subtree(root);
				return io_status::format;
			}

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

		void print (const int r, FILE *fp = stdout) const { print_subtree(root, 0, r, fp); }

		avl_tree_node<T> * find (const char *x) const
		{
			if (x == nullptr)
				return nullptr;

			avl_tree_node<T> *curr = root;
			while (curr)
			{
				int cmp = curr->cmp(x);

				if (cmp == 0)
					break;
				
				if (cmp < 0)
					curr = curr->right;

				else
					curr = curr->left;
			}
			
			return curr;
		}

	private:
		void add_node (avl_tree_node<T> *x)
		{
			if (!root)
				root = x;
			else
			{
				int incr = 0;
				root = add_avl_subtree(root, x, &incr);
			}
		}

		// curr != nullptr
		static avl_tree_node<T> * add_avl_subtree (avl_tree_node<T> *curr, avl_tree_node<T> *x, int *incr)
		{
			if (*x < *curr)
			{
				if (!curr->left)
				{
					curr->left = x;
					if (curr->right == nullptr)
					{
						curr->balance = -1;
						*incr = 1;
					} else
						curr->balance = 0;
				} else
				{
					curr->left = add_avl_subtree(curr->left, x, incr);
					curr = fix_left_tree(curr, incr);
				}
			} else
			{
				if (!curr->right)
				{
					curr->right = x;
					if (curr->left == nullptr)
					{
						curr->balance = 1;
						*incr = 1;
					} else
						curr->balance = 0;
				} else
				{
					curr->right = add_avl_subtree(curr->right, x, incr);
					curr = fix_right_tree(curr, incr);
				}
			}

			return curr;
		}

		static avl_tree_node<T> * fix_left_tree (avl_tree_node<T> *curr, int *incr)
		{
			if (*incr == 0)
				return curr;
			
			avl_tree_node<T> *temp = nullptr;

			if (curr->balance == 1)
			{
				curr->balance = 0;
				*incr = 0;

				return curr;
			} else if (curr->balance == 0)
			{
				curr->balance = -1;
				*incr = 1;

				return curr;
			}

			// По Валединскому случая с нулевым балансом не может быть

			if (curr->left->balance == -1)
			{
				temp = curr;
				curr = temp->left;
				temp->left = curr->right;
				curr->right = temp;

				curr->balance = 0;
				temp->balance = 0;
				*incr = 0;
			} else if (curr->left->balance == 1)
			{
				temp = curr;
				curr = curr->left->right;
				
				temp->left->right = curr->left;
				curr->left = temp->left;
				temp->left = curr->right;
				curr->right = temp;

				if (curr->balance == 1)
				{
					temp->balance = 0;
					curr->left->balance = -1;
				} else if (curr->balance == -1)
				{
					temp->balance = 1;
					curr->left->balance = 0;
				} else if (curr->balance == 0)
					temp->balance = curr->left->balance = 0;

				curr->balance = 0;
				*incr = 0;
			}

			return curr;
		}

		static avl_tree_node<T> * fix_right_tree (avl_tree_node<T> *curr, int *incr)
		{
			if (*incr == 0)
				return curr;
			
			avl_tree_node<T> *temp = nullptr;

			if (curr->balance == -1)
			{
				curr->balance = 0;
				*incr = 0;

				return curr;
			} else if (curr->balance == 0)
			{
				curr->balance = 1;
				*incr = 1;

				return curr;
			}

			// По Валединскому случая с нулевым балансом не может быть

			if (curr->right->balance == 1)
			{
				temp = curr;
				curr = curr->right;
				temp->right = curr->left;
				curr->left = temp;

				curr->balance = 0;
				temp->balance = 0;
				*incr = 0;
			} else if (curr->right->balance == -1)
			{
				temp = curr;
				curr = curr->right->left;
				
				temp->right->left = curr->right;
				curr->right = temp->right;
				temp->right = curr->left;
				curr->left = temp;

				if (curr->balance == -1)
				{
					temp->balance = 0;
					curr->right->balance = 1;
				} else if (curr->balance == 1)
				{
					temp->balance = -1;
					curr->right->balance = 0;
				} else if (curr->balance == 0)
					temp->balance = curr->right->balance = 0;

				curr->balance = 0;
				*incr = 0;
			}

			return curr;
		}

		static void delete_subtree (avl_tree_node<T> *root)
		{
			for (avl_tree_node<T> *child; root ; root = child)
			{
				child = root->right;

				delete_subtree(root->left);
				delete root;
			}
		}

		static void print_subtree (const avl_tree_node<T> *curr, int level, int r, FILE *fp = stdout)
		{
            curr->print(fp, level);
			
			if (level + 1 > r)
				return;

			if (curr->left)
				print_subtree(curr->left, level + 1, r, fp);

			if (curr->right)
				print_subtree(curr->right, level + 1, r, fp);
        }
};

#endif // AVL_TREE_H
