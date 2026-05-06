#ifndef SOLVE_H
#define SOLVE_H

#include <set>
#define LEN 1234

#include "io_status.h"

#include <cstdio>
#include <string>
#include <utility>
#include <algorithm>
#include <vector>

using lines_t = std::set<std::string>;

io_status read_print (FILE *in, FILE *out, lines_t& obj);
io_status t1_solve (const char *f_in, const char *f_out, int& r);

#endif // SOLVE_H
