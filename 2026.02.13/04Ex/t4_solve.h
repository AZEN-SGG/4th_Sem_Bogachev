#pragma once

#include "avl_tree.h"

const int MAX_LEVEL = 120;

template <typename T>
void avl_tree<T>::number_elems_from_string_on_level (const avl_tree_node<T> *curr, const char *s, int level, int *levels)
{
	for (; (curr && level < MAX_LEVEL); curr = curr->right, level++)
	{
		levels[level] += (is_from_set(curr->name.get(), s) > 0);

		if (curr->left)
			number_elems_from_string_on_level(curr->left, s, level + 1, levels);
	}
}

template <typename T>
int avl_tree<T>::t4_solve (const char * s) const
{
	if (root == nullptr)
		return 0;
	
	int levels[MAX_LEVEL] = {};

	number_elems_from_string_on_level(root, s, 0, levels);
	
	int max = 0;
	for (int level = 0 ; level < MAX_LEVEL ; level++)
		max = (max < levels[level]) ? levels[level] : max;

	return max;
}

