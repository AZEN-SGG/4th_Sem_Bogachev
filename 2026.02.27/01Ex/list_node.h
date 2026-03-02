#ifndef LIST_NODE_H
#define LIST_NODE_H

#include "student.h"

template <typename T>
class list2;

template <typename T>
class list2_node : public T
{
	private:
		list2_node * next = nullptr;
		list2_node * prev = nullptr;
	public:
		list2_node () = default;
		~list2_node () { next = nullptr; prev = nullptr; }

		list2_node (const list2_node&) = delete;
		list2_node (list2_node&& r) : T((T&&)r)
		{
			prev = r.prev;
			r.prev = nullptr;

			next = r.next;
			r.next = nullptr;
		}

		list2_node& operator= (const list2_node&) = delete;
		list2_node& operator= (list2_node&& r)
		{
			if (this == &r)
				return *this;

			*(T *)(this) = (T&&)r;
			
			prev = r.prev;
			r.prev = nullptr;

			next = r.next;
			r.next = nullptr;
			return *this;
		}

		list2_node * get_next () const { return next; }
		list2_node * get_prev () const { return prev; }

		void set_next (list2_node *r) { next = r; }
		void set_prev (list2_node *r) { prev = r; }

		friend class list2<T>;
};

extern template class list2_node<student>;

#endif // LIST_NODE_H
