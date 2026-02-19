#pragma once

#include "avl_tree.h"

template <typename T>
int avl_tree<T>::get_max_number_elem_with_name_in_branch_in_subtree (const avl_tree_node<T> *curr, const char * s)
{
	int number = 0,
		max = 0;

	if (curr->left)
		max = get_max_number_elem_with_name_in_branch_in_subtree(curr->left, s);

	if (curr->right)
		number = get_max_number_elem_with_name_in_branch_in_subtree(curr->right, s);

	max = (max < number) ? number : max;

	if (is_from_set(curr->name.get(), s) > 0)
		max++;

	return max;
}

template <typename T>
int avl_tree<T>::t2_solve (const char * s) const
{
	if (root == nullptr)
		return 0;
	
	return get_max_number_elem_with_name_in_branch_in_subtree(root, s);
}

