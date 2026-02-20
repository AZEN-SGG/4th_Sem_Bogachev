#ifndef RB_TREE_NODE_H
#define RB_TREE_NODE_H

#include <cstdio>

template <typename T>
class rb_tree;

template <typename T>
class rb_tree_node : public T
{
	enum class colors
	{
		invalid,
		red,
		black,
	};
	private:
		rb_tree_node *left = nullptr;
		rb_tree_node *right = nullptr;
		rb_tree_node *parent = nullptr;
		colors color = colors::invalid;

	public:
		rb_tree_node () = default;
		~rb_tree_node () { erase_links(); }

		rb_tree_node (const rb_tree_node &x) = delete;
		rb_tree_node (rb_tree_node &&x) : T((T&&)x)
		{
			erase_links();
			x.erase_links();
		}

		rb_tree_node(const T &x) = delete;
		rb_tree_node(T &&x) : T(std::move(x)) { erase_links(); }

		rb_tree_node & operator= (const rb_tree_node &x) = delete;
		rb_tree_node & operator= (rb_tree_node &&x)
		{
			if (this == &x)
				return *this;
			*(T *)this = (T&&)x;

			left = x.left;
			right = x.right;
			color = x.color;

			x.erase_links();
			return *this;
		}

		void print (FILE * fp = stdout) const
		{
			switch (color)
			{
				case colors::red:
					fprintf(fp, "red     ");
					break;
				case colors::black:
					fprintf(fp, "black   ");
					break;
				case colors::invalid:
					fprintf(fp, "invalid ");
					break;
			};
			static_cast<const T*>(this)->print(fp);
		}

		friend class rb_tree<T>;
	
	private:
		void erase_links ()
		{
			left = nullptr;
			right = nullptr;
			color = colors::invalid;
		}
};

#endif // RB_TREE_NODE_H
