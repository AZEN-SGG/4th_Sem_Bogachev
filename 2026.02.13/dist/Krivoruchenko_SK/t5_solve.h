#pragma once

#include "avl_tree.h"

template <typename T>
int avl_tree<T>::get_diff_btwn_num_elem_from_str_in_subtrees (const avl_tree_node<T> *curr, const char *s, int *max)
{
	int left_n = 0,
		right_n = 0;

	if (curr->left)
		left_n = get_diff_btwn_num_elem_from_str_in_subtrees(curr->left, s, max);

	if (curr->right)
		right_n = get_diff_btwn_num_elem_from_str_in_subtrees(curr->right, s, max);

	int diff = left_n - right_n;
	diff = diff < 0 ? -diff : diff;
	*max = (*max < diff) ? diff : *max;

	left_n += right_n;
	left_n += (is_from_set(curr->name.get(), s) > 0);

	return left_n;
}

template <typename T>
int avl_tree<T>::t5_solve (const char * s) const
{
	if (root == nullptr)
		return 0;
	
	int max = 0;
	get_diff_btwn_num_elem_from_str_in_subtrees(root, s, &max);

	return max;
}

