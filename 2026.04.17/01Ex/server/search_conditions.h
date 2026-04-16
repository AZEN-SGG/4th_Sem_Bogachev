#ifndef SEARCH_CONDITIONS
#define SEARCH_CONDITIONS

#include "operation.h"
#include "condition.h"
#include "validator.h"
#include "ordering.h"

template <typename U>
class index_trees;

template <typename U, ordering X>
class index_array;

template <typename U>
class search_structure_store;

template <typename T>
class search_conditions : public T
{
protected:
	condition c_name	= condition::none;
	condition c_phone	= condition::none;
	condition c_group	= condition::none;
	operation op		= operation::none;

public:
	search_conditions () = default;
	~search_conditions () = default;

	search_conditions (search_conditions&&) noexcept = default;
	search_conditions& operator= (search_conditions&&) noexcept = default;

	search_conditions (const search_conditions&) = delete;
	search_conditions& operator= (const search_conditions&) = delete;
	
	template <typename X>
	void make_validator (validator<search_conditions<T>, X>& val) const;

	template <typename X>
	bool cmp_group (X& x) const { return x.compare_group(c_group, *this); }

	template <typename X>
	bool cmp_phone (X& x) const { return x.compare_phone(c_phone, *this); }

	template <typename X>
	bool cmp_name (X& x) const { return x.compare_word(c_name, *this); }

	template <typename U>
	friend class index_trees;

	template <typename U, ordering X>
	friend class index_array;

	template <typename U>
	friend class search_structure_store;

	template <typename U>
	friend class database;
};

#endif // SEARCH_CONDITIONS
