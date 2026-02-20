#ifndef SOLVE_H
#define SOLVE_H

#include "avl_tree.h"
#include "pair.h"

#include <memory>

#define CHAR_SIZE 256

io_status replace_words (FILE *in, FILE *out, const avl_tree<pair> *olha, const bool hash[], int *res);
io_status solve_03 (char *f_in, char *f_out, char *s, char *x, const char *t, int *r);

#endif // SOLVE_H
