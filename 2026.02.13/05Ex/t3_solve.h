#pragma once

#include "avl_tree.h"

template <typename T>
int avl_tree<T>::get_number_subtrees_with_elem_from_string (const avl_tree_node<T> *curr, const char *s, int *elem)
{
	int suit = 0,
		number = 0;

	if (curr->left)
		suit = get_number_subtrees_with_elem_from_string(curr->left, s, &number);

	if (curr->right)
		suit += get_number_subtrees_with_elem_from_string(curr->right, s, &number);

	if ((suit == number) && (is_from_set(curr->name.get(), s) > 0))
		suit++;
	number++;

	*elem += number;

	return suit;
}

template <typename T>
int avl_tree<T>::t3_solve (const char * s) const
{
	if (root == nullptr)
		return 0;
	
	int number = 0;

	return get_number_subtrees_with_elem_from_string(root, s, &number);
}

