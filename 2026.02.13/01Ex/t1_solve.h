#pragma once

#include "rb_tree.h"

template <typename T>
int rb_tree<T>::get_number_elem_in_subtrees_with_k_nodes (const rb_tree_node<T> *curr, const int k)
{
	int number = 1;

	if (curr->left)
		number += get_number_elem_in_subtrees_with_k_nodes(curr->left, k);

	if (curr->right)
		number += get_number_elem_in_subtrees_with_k_nodes(curr->right, k);

	if (number > k)
		number--;

	return number;
}

template <typename T>
int rb_tree<T>::t1_solve (const int k) const
{
	if (!root || k < 1)
		return 0;
	
	return get_number_elem_in_subtrees_with_k_nodes(root, k);
}

