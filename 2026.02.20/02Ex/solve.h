#ifndef SOLVE_H
#define SOLVE_H

#include "rb_tree.h"
#include "line.h"

#include <memory>

int inclusion (char * str, const rb_tree<line> * olha, const char * delim);
io_status find_inclusion (FILE * in, FILE * out, const rb_tree<line> * olha, const char * t, int * res);
io_status solve_02 (char * f_in, char * f_out, char * s, const char * t, int * r);

#endif // SOLVE_H
