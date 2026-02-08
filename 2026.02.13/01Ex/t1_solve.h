#pragma once

#include "avl_tree.h"

template <typename T>
int avl_tree<T>::get_number_end_elem_with_name_in_subtree (const avl_tree_node<T> *curr, const char * s)
{
	int number = 0;

	if (curr->left)
		number += get_number_end_elem_with_name_in_subtree(curr->left, s);

	if (curr->right)
		number += get_number_end_elem_with_name_in_subtree(curr->right, s);

	else
		if (curr->left == nullptr)
			number += (is_from_set(curr->name.get(), s) != 0);

	return number;
}

template <typename T>
int avl_tree<T>::t1_solve (const char * s) const
{
	if (root == nullptr)
		return 0;
	
	return get_number_end_elem_with_name_in_subtree(root, s);
}

