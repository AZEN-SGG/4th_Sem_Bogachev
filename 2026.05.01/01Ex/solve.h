#ifndef SOLVE_H
#define SOLVE_H

#define LEN 1234

#include "io_status.h"

#include <cstdio>
#include <string>
#include <utility>
#include <vector>

using vec_t = std::vector<std::pair<std::string, int>>;

template <typename T>
io_status read_file (const char *f_in, T& obj)
{
	FILE *in = fopen(f_in, "r");
	if (in)
		return io_status::open;

	return read(in, obj);
}

io_status read (FILE *in, vec_t& obj);

io_status t1_solve (const char *f_in, const char *f_out, int& r);

#endif // SOLVE_H
