#ifndef SOLVE_H
#define SOLVE_H

#include "b_tree.h"
#include "line.h"

#include <memory>

int intersect (char * str, const b_tree<line> * olha, const char * delim);
io_status find_intersection (FILE * in, FILE * out, const b_tree<line> * olha, const char * t, int * res);
io_status solve_01 (char * f_in, char * f_out, char * s, const char * t, int m, int * r);

#endif // SOLVE_H
