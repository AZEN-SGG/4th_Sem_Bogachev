#ifndef SOLVE_H
#define SOLVE_H

#include <list>
#define LEN 1234

#include "io_status.h"

#include <cstdio>
#include <string>
#include <utility>
#include <algorithm>
#include <vector>

using line_t = std::pair<std::string, int>;
using dupl_t = std::pair<std::string, std::pair<int, int>>;

using vec_t = std::vector<line_t>;
using list_t = std::list<line_t>;

using dvec_t = std::vector<dupl_t>;

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

io_status read (FILE *in, list_t& obj);
int print (FILE *out, const list_t& obj) noexcept;

int delete_duplicates (list_t& lines);
io_status t2_solve (const char *f_in, const char *f_out, int& r);

io_status read (FILE *in, dvec_t& obj);
int print (FILE *out, const dvec_t& obj) noexcept;

int number_duplicates (dvec_t& vec);
io_status t3_solve (const char *f_in, const char *f_out, int& r);

#endif // SOLVE_H
