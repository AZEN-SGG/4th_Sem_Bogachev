#ifndef LIST_NODE_H
#define LIST_NODE_H

template <typename T>
class list;

template <typename X, typename U>
class data_tree;

template <typename T>
class list_node : public T
{
private:
	list_node *next = nullptr;
public:
	list_node () = default;
	~list_node () { next = nullptr; }

	list_node (const list_node&) = delete;
	list_node (list_node&& r) : T((T&&)r) 
	{ 
		next = r.next;
		r.next = nullptr;
	}

	list_node& operator= (const list_node&) = delete;
	list_node& operator= (list_node&& r)
	{
		*(T *)(this) = (T&&)r;

		next = r.next;
		r.next = nullptr;
		return *this;
	}

	list_node* get_next () const { return next; }
	void set_next (list_node *r) { next = r; }

	friend class list<T>;
	template <typename X, typename U>
	friend class data_tree;
};

#endif // LIST_NODE_H
