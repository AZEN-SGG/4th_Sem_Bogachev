#ifndef SOLVE_H
#define SOLVE_H

#include "list.h"
#include "pattern.h"
#include "io_status.h"

#include <memory>
#include <cstdio>

io_status write_sentence_with_right_percent (FILE *in, FILE *out, const list<pattern> *shai_hulud, const char *t, int *res);
io_status solve_07 (char *f_in, char *f_out, char *s, const char *t, int *r);

#endif // SOLVE_H
