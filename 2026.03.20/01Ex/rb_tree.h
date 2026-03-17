#ifndef RB_TREE_H
#define RB_TREE_H

#include "io_status.h"
#include "rb_tree_node.h"

template <typename T>
class rb_tree
{
	private:
		rb_tree_node<T> *root = nullptr;
	
	public:
		rb_tree () = default;
		~rb_tree ()
		{
			delete_subtree(root);
			root = nullptr;
		}
	
		io_status read (FILE *fp, const unsigned int max_read = -1)
		{
			rb_tree_node<T> x;
			io_status ret;

			if (x.read(fp) != (ret = io_status::success))
				return ret;

			root = new rb_tree_node<T>((rb_tree_node<T>&&)x);
			if (root == nullptr)
				return io_status::memory;

			unsigned int readed = 1;
			while ((x.read(fp) == io_status::success) && (readed < max_read))
			{
				rb_tree_node<T> *node = new rb_tree_node<T>((rb_tree_node<T>&&)x);
				if (!node)
				{
					delete_subtree(root);
					return io_status::memory;
				}

				add_node(node);
				fix_tree(node);
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

		bool delete_node (T& x)
		{
			if (!root)
				return false;
			
			
		}

		void print (const int r, FILE *fp = stdout) const {	print_subtree(root, 0, r, fp); }
	private:
		void add_node (rb_tree_node<T> *x)
		{
			if (!root)
			{
				root = x;
				root->color = rb_tree_node<T>::colors::black;
			} else
				add_rb_subtree(root, x);
		}

		// curr != nullptr
		static void add_rb_subtree (rb_tree_node<T> *curr, rb_tree_node<T> *x)
		{
			if (*x < *curr)
			{
				if (!curr->left)
				{
					curr->left = x;
					x->parent = curr;
					x->color = rb_tree_node<T>::colors::red;

				} else
					add_rb_subtree(curr->left, x);
			} else
			{
				if (!curr->right)
				{
					curr->right = x;
					x->parent = curr;
					x->color = rb_tree_node<T>::colors::red;
				} else
					add_rb_subtree(curr->right, x);
			}
		}

		void fix_tree (rb_tree_node<T> *x)
		{
			rb_tree_node<T> *parent, *grandparent;

			while (x->parent != nullptr &&
					x->color == rb_tree_node<T>::colors::red &&
					x->parent->color == rb_tree_node<T>::colors::red
			) {
				// parent != nullptr!
				parent = x->parent;
				grandparent = parent->parent;
				if (parent == grandparent->left)
				{
					// uncle красный
					if (grandparent->right && 
						grandparent->right->color == rb_tree_node<T>::colors::red)
					{
						grandparent->right->color = parent->color = rb_tree_node<T>::colors::black;
						if (grandparent->parent)
							grandparent->color = rb_tree_node<T>::colors::red;

						x = grandparent;
					} else
					{
						// grandparent становится правым поддеревом
						rb_tree_node<T> *new_root = nullptr;
						// Приводим к левому случаю
						if (parent->right == x)
						{
							new_root = x;
							
							parent->right = x->left;
							if (x->left)
								x->left->parent = parent;

							x->left = parent;
							parent->parent = x;

							grandparent->left = x;
							x->parent = grandparent;
						} else
						{
							new_root = parent;
							x = parent;
						}

						if (grandparent->parent)
						{
							if (grandparent->parent->left == grandparent)
								grandparent->parent->left = new_root;
							else
								grandparent->parent->right = new_root;
						}
						new_root->parent = grandparent->parent;

						grandparent->left = new_root->right;
						if (new_root->right)
							new_root->right->parent = grandparent;

						new_root->right = grandparent;
						grandparent->parent = new_root;

						new_root->color = rb_tree_node<T>::colors::black;
						grandparent->color = rb_tree_node<T>::colors::red;
					}
				} else // зеркально для правого случая
				{
					// uncle красный
					if (grandparent->left && 
						grandparent->left->color == rb_tree_node<T>::colors::red)
					{
						grandparent->left->color = parent->color = rb_tree_node<T>::colors::black;
						if (grandparent->parent)
							grandparent->color = rb_tree_node<T>::colors::red;

						x = grandparent;
					} else
					{
						// grandparent становится правым поддеревом
						rb_tree_node<T> *new_root = nullptr;
						if (parent->left == x)
						{
							new_root = x;

							parent->left = x->right;
							if (x->right)
								x->right->parent = parent;

							x->right = parent;
							parent->parent = x;

							grandparent->right = x;
							x->parent = grandparent;
						} else
						{
							new_root = parent;
							x = parent;
						}

						grandparent->right = new_root->left;
						if (new_root->left)
							new_root->left->parent = grandparent;

						if (grandparent->parent)
						{
							if (grandparent->parent->left == grandparent)
								grandparent->parent->left = new_root;
							else
								grandparent->parent->right = new_root;
						}
						new_root->parent = grandparent->parent;

						new_root->left = grandparent;
						grandparent->parent = new_root;

						new_root->color = rb_tree_node<T>::colors::black;
						grandparent->color = rb_tree_node<T>::colors::red;
					}
				}
			}
			
			if (!x->parent)
				root = x;
		}

		static rb_tree_node<T> * search_delete (const T& x, rb_tree_node<T> *curr, rb_tree_node<T> *prev)
		{
			if (!curr)
				return prev;

			int cmp = 0;
			if ((cmp = x.cmp(*curr)) < 0)
			{
				curr = search_delete(x, curr->left, curr);
			} else if (cmp > 0)
			{
				curr = search_delete(x, curr->right, curr);
			} else
			{
				if (curr->left)
				{
					curr = delete_recurrently(curr->left, curr, curr);
				} else
				{
					if (curr->right)
					{
						rb_tree_node<T> *temp = curr->right;
						curr->left = temp->left;
						curr->right = temp->right;
						curr = static_cast<T&&>(temp);

						delete temp;
						prev = fix_delete(curr, prev);
					} else
					{
						if (curr == prev->left)
							prev->left = nullptr;
						else
							prev->right = nullptr;

						prev = fix_delete(curr, prev);
						delete curr;
					}
				}
			}


			return prev;
		}

		static rb_tree_node<T> * delete_recurrently (rb_tree_node<T> *curr, rb_tree_node<T> *prev, rb_tree_node<T> *removable)
		{
			if (curr->right)
			{
				curr = delete_recurrently(curr->right, curr);
				prev = fix_delete(curr, prev);
			} else
			{
				// Переместили концевой на место удаляемого
				*removable = static_cast<T&&>(*curr);
				
				if (curr->left)
				{
					rb_tree_node<T> *temp = curr->left;
					*curr = static_cast<T&&>(*curr->left);
					delete temp;

					// Перестраиваем с ребёнком
					prev = fix_delete(curr, prev);
				} else
				{
					// Перестраиваем с фиктивным ребёнком
					prev = fix_delete(curr, prev);
					
					if (prev->left == curr)
						prev->left = nullptr;
					else
						prev->right = nullptr;

					delete curr;
					curr = nullptr;
				}
			}
			
			return prev;
		}

		static rb_tree_node<T> * fix_delete (rb_tree_node<T> *curr, rb_tree_node<T> *prev)
		{

		}

		static void delete_subtree (rb_tree_node<T> *child)
		{
			while (child)
			{
				if (child->left)
					child = child->left;
				else if (child->right)
					child = child->right;
				else
				{
					rb_tree_node<T> *parent = child->parent;
					delete child;
					
					if (!parent)
						break;

					if (parent->left == child)
						parent->left = nullptr;
					else
						parent->right = nullptr;
					
					child = parent;
				}
			}
		}

		static void print_subtree (const rb_tree_node<T> *curr, int level, int r, FILE *fp = stdout)
		{
			int flag = 0;
			while (curr)
			{
				if (level <= r && flag != 2)
				{
					if (flag == 0)
					{
						for (int i = 0 ; i < level ; i++)
							fprintf(fp, "  ");
						curr->print(fp);
					}

					if (curr->left && flag != 1)
					{
						curr = curr->left;
						level++;
						flag = 0;
					} else if (curr->right)
					{
						curr = curr->right;
						level++;
						flag = 0;
					} else
						flag = 2;
				} else
				{
					if (!curr->parent)
						return;

					if (curr->parent->right == curr)
						flag = 2;
					else
					{
						if (curr->parent->right)
							flag = 1;
						else
							flag = 2;
					}

					curr = curr->parent;
					level--;
				}
					
			}
		}
};

#endif // RB_TREE_H
