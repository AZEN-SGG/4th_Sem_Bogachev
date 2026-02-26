#ifndef SOLVE_H
#define SOLVE_H

#include "list.h"
#include "operation.h"

#include <memory>
#include <cstdio>

io_status write_sentence_where_all (FILE *in, FILE *out, const list<operation> *shai_hulud, const char *t, int *res);
io_status solve_05 (char *f_in, char *f_out, char *s, char *x, const char *t, int *r);

#endif // SOLVE_H
