#include "list.h"

// Adding sorted list B to sorted list A
list_node * list::add_sorted_to_sorted (list_node *l_temp, int len_add, list_node **end_A)
{
    list_node *added = nullptr,
              *added_next = nullptr,
              *start = head, // Start for adding into list
              *prev_start = nullptr;

    int i;
    for (added = l_temp, i = 0 ; ((added != nullptr) && (i < len_add)) ; added = added_next, ++i)
    {
        list_node *curr = nullptr,
                  *prev = nullptr;

        added_next = added->next;

        for (prev = prev_start, curr = start ; (curr != nullptr) && (*curr < *added) ; prev = curr, curr = curr->next);
        if (curr == nullptr)
            *end_A = added;

        if (prev == nullptr)
            head = added;
        else
            prev->next = added;
        
        added->next = curr;
        
        start = curr;
        prev_start = added;
    }

    return added;
}

void list::neyman_sort ()
{
    int k = 0,
        n = 1;

    // Main loop
    while (k != 1)
    {
        k = 0;

        list_node *start = nullptr,
                  *sorted_end = nullptr;
        list sorted;

        for (start = head ; (start != nullptr) ; k++)
        {
            list A;
            list_node *end_A = nullptr,
                      *start_B = nullptr;

            // Searching end of A
            int i;
            for (i = 0, end_A = start ; (end_A != nullptr) && (i < (n - 1)) ; ++i, end_A = end_A->next);
            
            // Length of A less than n
            if (end_A == nullptr)
            {
                if (sorted_end == nullptr)
                    sorted.head = head;
                else
                    sorted_end->next = start;

                start = nullptr;
                // Can add sorted_end = nullptr, but it is not necessity
                continue;
            }
    
            // Init A
            A.head = start;
            start_B = end_A->next;
            end_A->next = nullptr; // End of the list must be nullptr
    
            // Adding B into A
            start = A.add_sorted_to_sorted(start_B, n, &end_A);
            
            if (sorted_end == nullptr)
                sorted.head = A.head;
            else
                sorted_end->next = A.head;
            
            sorted_end = end_A;

            A.head = nullptr;
        }

        head = nullptr;
        *this = (list&&)sorted;

        n <<= 1;
    }
}
