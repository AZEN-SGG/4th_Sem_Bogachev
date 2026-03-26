#ifndef RB_TREE_H
#define RB_TREE_H

#include "io_status.h"
#include "rb_tree_node.h"

#include <new>

class database;

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

	template <typename X>
	void delete_node (const X& x)
	{
		auto curr = search_node<X>(x);

		if (!curr)
			return;

		// Значит равен, начинаем удаление
		// Если есть левый потомок, то ищем самый большой из меньших
		if (curr->left)
		{
			if (!curr->left->right)
			{
				curr = curr->left;

				static_cast<T&>(*curr->parent) = static_cast<T&&>(*curr);
				
				if (curr->color == rb_tree_node<T>::colors::black)
				{
					if (rb_tree_node<T>::get_color(curr->left) == rb_tree_node<T>::colors::black)
					{
						if (curr->left)
						{
							rb_tree_node<T> *temp = nullptr;
							curr->parent->left = curr->left;
							curr->left->parent = curr->parent;

							temp = curr->left;
							delete curr;
							curr = temp;

							rebuild_deletion(curr);
						} else
						{
							rebuild_deletion(curr);
							delete curr;
						}
					} else
					{
						rb_tree_node<T> *temp = nullptr;
						curr->parent->left = curr->left;
						curr->left->parent = curr->parent;

						temp = curr->left;
						delete curr;
						curr = temp;

						curr->color = rb_tree_node<T>::color::black;
					}
				// Удаляемая вершина - красная, значит просто переносим left
				} else
				{
					if (curr->left)
					{
						curr->parent->left = curr->left;
						curr->left->parent = curr->parent;
					}
				}
			} else
			{
				rb_tree_node<T> *temp = curr;
				for (curr = curr->left->right; curr->right ; curr = curr->right);

				static_cast<T&>(*temp) = static_cast<T&&>(*curr);
				
				if (curr->color == rb_tree_node<T>::colors::black)
				{
					if (rb_tree_node<T>::get_color(curr->left) == rb_tree_node<T>::colors::black)
					{
						if (curr->left)
						{
							curr->parent->right = curr->left;
							curr->left->parent = curr->parent;

							temp = curr->left;
							delete curr;
							curr = temp;

							rebuild_deletion(curr);
						} else
						{
							rebuild_deletion(curr);
							delete curr;
						}
					} else
					{
						curr->parent->right = curr->left;
						curr->left->parent = curr->parent;
						curr->left->color = rb_tree_node<T>::color::black;

						delete curr;
						curr = nullptr;
					}
				// Удаляемая вершина - красная, значит просто переносим left
				} else
				{
					if (curr->left)
					{
						curr->parent->right = curr->left;
						curr->left->parent = curr->parent;
					}
				}
			}
		// У удаляемой нет левого потомка, значит удаляем именно неё!
		} else
		{
			if (curr->color == rb_tree_node<T>::colors::black)
			{
				if (rb_tree_node<T>::get_color(curr->right) == rb_tree_node<T>::colors::black)
				{
					if (curr->right)
					{
						curr->right->parent = curr->parent;

						if (curr->parent)
						{
							if (curr == curr->parent->left)
								curr->parent->left = curr->right;
							else
								curr->parent->right = curr->right;

							rb_tree_node<T> *temp = curr->right;
							delete curr;
							curr = temp;

							rebuild_deletion(curr);
						} else
						{
							root = curr->right;
							delete curr;
							curr = nullptr;
						}
					} else
					{
						if (curr->parent)
						{
							if (curr == curr->parent->left)
								curr->parent->left = nullptr;
							else
								curr->parent->right = nullptr;

							rebuild_deletion(curr);
						} else
							root = nullptr;

						delete curr;
						curr = nullptr;
					}
				// Значит ребёнок есть и он красный!
				} else
				{
					if (curr->parent)
					{
						if (curr == curr->parent->left)
							curr->parent->left = curr->right;
						else
							curr->parent->right = curr->right;
					} else
						root = curr->right;
					curr->right->parent = curr->parent;
					curr->right->color = rb_tree_node<T>::color::black;

					delete curr;
					curr = nullptr;
				}
			// Удаляемая - красная
			} else
			{
				if (curr == curr->parent->left)
					curr->parent->left = curr->right;
				else
					curr->parent->right = curr->right;

				if (curr->right)
					curr->Right->parent = curr->parent;

				delete curr;
				curr = nullptr;
			}
		}
	}

	// Поиск элемента
	template <typename X>
	rb_tree_node<T> * search_node (const X& x)
	{
		rb_tree_node<T> *curr = root;
		while (curr)
		{
			// Сравниваем
			int cmp = curr->cmp(x);

			// Если меньше, то в левое
			if (cmp < 0)
				curr = curr->left;
			// Если больше, то в правое
			else if (cmp > 0)
				curr = curr->right;
			else
				break;
		}

		// Если пуст, то не нашли
		return curr;
	}

	// На вход подаётся list2_node
	template <typename X>
	io_status add (X *x)
	{
		auto curr = root;
		while (true)
		{
			int cmp = curr->cmp(*x);

			if (cmp > 0)
			{
				if (!curr->left)
				{
					auto temp = new (std::nothrow) rb_tree_node<T>();
					if (!temp)
						return io_status::memory;

					curr->left = temp;
					temp->parent = curr;
					temp->color = rb_tree_node<T>::colors::red;

					curr = temp;
					break;
				} else
					curr = curr->left;
			} else if (cmp < 0)
			{
				if (!curr->right)
				{
					auto temp = new (std::nothrow) rb_tree_node<T>();
					if (!temp)
						return io_status::memory;

					curr->right = temp;
					temp->parent = curr;
					temp->color = rb_tree_node<T>::colors::red;

					curr = temp;
					break;
				} else
					curr = curr->right;
			} else
				break;
		}
		
		return curr->add(x);
	}

	void print (const int r, FILE *fp = stdout) const {	print_subtree(root, 0, r, fp); }

	friend class database;
private:
	void erase () { delete_subtree(root); root = nullptr; }

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
		int cmp = x->cmp(*curr);

		if (cmp < 0)
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

	// Перестройка дерева
	static void rebuild_deletion (rb_tree_node<T> *curr)
	{
		rb_tree_node<T> *temp = curr->parent;
		for (; curr->parent ; curr = curr->parent)
		{
			if (curr == curr->parent->left)
			{
				// 1й Случай: Брат - краснокожий :(
				if (curr->parent->right->color == rb_tree_node<T>::colors::red)
				{
					// Обмен между B и D
					if (curr->parent->parent)
					{
						if (curr->parent == curr->parent->parent->left)
							curr->parent->parent->left = curr->parent->right;
						else
							curr->parent->parent->right = curr->parent->right;
					}
					curr->parent->right->parent = curr->parent->parent;

					temp = curr->parent->right;
					curr->parent->right = temp->left;
					if (temp->left)
						temp->left->parent = curr->parent;

					temp->left = curr->parent;
					curr->parent->parent = temp;

					temp->color = rb_tree_node<T>::colors::black;
					curr->parent->color = rb_tree_node<T>::colors::red;
				}

				// Теперь "брат" - обязательно чёрный!
				// Не брат ты мне гнида черномазая

				if (rb_tree_node<T>::get_color(curr->parent->right->right) == rb_tree_node<T>::colors::black)
				{
					// 2е Преобразование
					if (rb_tree_node<T>::get_color(curr->parent->right->left) == rb_tree_node<T>::colors::black)
					{
						curr->parent->right->color = rb_tree_node<T>::colors::red;
						curr = rb_tree_node<T>::colors::black;

						if (curr->parent->color == rb_tree_node<T>::colors::red)
						{
							curr->parent->color = rb_tree_node<T>::colors::black;
							break;
						}

						continue;
					// 3е Преобразование
					// Красная может быть только у существующей
					} else
					{
						temp = curr->parent->right->left;
						temp->parent = curr->parent;
						curr->parent->right->left = temp->right;
						if (temp->right)
							temp->right->parent = curr->parent->right;
						temp->right = curr->parent->right;
						curr->parent->right = temp;
						temp->right->parent = temp;

						temp->color = rb_tree_node<T>::colors::black;
						temp->right->color = rb_tree_node<T>::colors::red;
					}
				}

				// Теперь правый ребёнок брата - обязательно красный!

				// 4е Преобразование
				temp = curr->parent->right;
				if (curr->parent->parent)
				{
					if (curr->parent == curr->parent->parent->left)
						curr->parent->parent->left = temp;
					else
						curr->parent->parent->right = temp;
				}
				temp->parent = curr->parent->parent;
				curr->parent->parent = temp;
				curr->parent->right = temp->left;
				if (temp->left)
					temp->left->parent = curr->parent;
				temp->left = curr->parent;

				temp->color = curr->parent->color;
				temp->right->color == rb_tree_node<T>::colors::black;
				curr->parent->color = rb_tree_node<T>::colors::black;

				break;
			} else
			{
				// 1й Случай: Брат - краснокожий :(
				if (curr->parent->left->color == rb_tree_node<T>::colors::red)
				{
					// Обмен между B и D
					if (curr->parent->parent)
					{
						if (curr->parent == curr->parent->parent->left)
							curr->parent->parent->left = curr->parent->left;
						else
							curr->parent->parent->right = curr->parent->left;
					}
					curr->parent->left->parent = curr->parent->parent;

					temp = curr->parent->left;
					curr->parent->left = temp->right;
					if (temp->right)
						temp->right->parent = curr->parent;

					temp->right = curr->parent;
					curr->parent->parent = temp;

					temp->color = rb_tree_node<T>::colors::black;
					curr->parent->color = rb_tree_node<T>::colors::red;
				}

				// Теперь "брат" - обязательно чёрный!
				// Не брат ты мне гнида черномазая

				if (rb_tree_node<T>::get_color(curr->parent->left->left) == rb_tree_node<T>::colors::black)
				{
					if (rb_tree_node<T>::get_color(curr->parent->left->right) == rb_tree_node<T>::colors::black)
					{
						curr->parent->left->color = rb_tree_node<T>::colors::red;
						curr = rb_tree_node<T>::colors::black;

						if (curr->parent->color == rb_tree_node<T>::colors::red)
						{
							curr->parent->color = rb_tree_node<T>::colors::black;
							break;
						}

						continue;
					// Красная может быть только у существующей
					} else
					{
						temp = curr->parent->left->right;
						temp->parent = curr->parent;
						curr->parent->left->right = temp->left;
						if (temp->left)
							temp->left->parent = curr->parent->left;
						temp->left = curr->parent->left;
						curr->parent->left = temp;
						temp->left->parent = temp;

						temp->color = rb_tree_node<T>::colors::black;
						temp->left->color = rb_tree_node<T>::colors::red;
					}
				}

				// Теперь правый ребёнок брата - обязательно красный!

				// 4е Преобразование
				temp = curr->parent->left;
				if (curr->parent->parent)
				{
					if (curr->parent == curr->parent->parent->left)
						curr->parent->parent->left = temp;
					else
						curr->parent->parent->right = temp;
				}
				temp->parent = curr->parent->parent;
				curr->parent->parent = temp;
				curr->parent->left = temp->right;
				if (temp->right)
					temp->right->parent = curr->parent;
				temp->right = curr->parent;

				temp->color = curr->parent->color;
				temp->left->color == rb_tree_node<T>::colors::black;
				curr->parent->color = rb_tree_node<T>::colors::black;

				break;
			}
		}
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
