#ifndef SOLVE_H
#define SOLVE_H

#define LEN 1234

#include "io_status.h"

#include <cstdio>
#include <string>
#include <utility>
#include <vector>

using vec_el_t = std::pair<std::string, int>;
using vec_t = std::vector<vec_el_t>;

template <typename T>
io_status read_file (const char *f_in, T& obj)
{
	FILE *in = fopen(f_in, "r");
	if (!in)
		return io_status::open;

	return read(in, obj);
}

template <typename T>
io_status print_file (const char *f_out, T& obj, int& r) noexcept
{
	FILE *out = fopen(f_out, "w");
	if (!out)
		return io_status::open;

	r = print(out, obj);

	return io_status::success;
}

io_status read (FILE *in, vec_t& obj);
int print (FILE *out, const vec_t& obj) noexcept;

int delete_duplicates (vec_t& vec);
io_status t1_solve (const char *f_in, const char *f_out, int& r);

#endif // SOLVE_H
