#ifndef SOLVE_H
#define SOLVE_H

#define LEN 1234

#include "io_status.h"

#include <cstdio>
#include <string>
#include <utility>
#include <algorithm>
#include <map>

using lines_t = std::map<std::string, int>;

int read_print (FILE *in, FILE *out, lines_t& obj);
io_status t4_solve (const char *f_in, const char *f_out, int& r);

#endif // SOLVE_H
