#ifndef SEARCH_CONDITIONS
#define SEARCH_CONDITIONS

#include "operation.h"
#include "condition.h"

template <typename T>
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

	friend class search_structure_store<T>;
};

#endif // SEARCH_CONDITIONS
