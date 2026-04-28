#ifndef AVL_TREE_NODE_H
#define AVL_TREE_NODE_H

#include <cstdio>

template <typename T>
class avl_tree;

template <typename T>
class avl_tree_node : public T
{
	private:
		avl_tree_node *left = nullptr;
		avl_tree_node *right = nullptr;
		int balance = 0;

	public:
		avl_tree_node () = default;
		~avl_tree_node () { erase_links(); }

		avl_tree_node (const avl_tree_node &x) = delete;
		avl_tree_node (avl_tree_node &&x) : T((T&&)x)
		{
			erase_links();
			x.erase_links();
		}

		avl_tree_node & operator= (const avl_tree_node &x) = delete;
		avl_tree_node & operator= (avl_tree_node &&x)
		{
			if (this == &x)
				return *this;
			*(T *)this = (T&&)x;

			left = x.left;
			right = x.right;
			balance = x.balance;

			x.erase_links();
			return *this;
		}

		void print (FILE * fp = stdout, int level = 0) const
		{
			for (int i = 0 ; i < level ; ++i)
				fprintf(fp, "  ");

			if (balance >= 0)
				fprintf(fp, " ");
			fprintf(fp, "%d ", balance);
			static_cast<const T*>(this)->print(fp);
		}

		friend class avl_tree<T>;
	
	private:
		void erase_links ()
		{
			left = nullptr;
			right = nullptr;
			balance = 0;
		}
};

#endif // AVL_TREE_NODE_H
