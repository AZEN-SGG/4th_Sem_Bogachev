#include "solve.h"
#include "io_status.h"
#include <utility>

io_status read (FILE *in, vec_t& vec)
{
	char BUF[LEN] = {};
	
	for (int num = 1 ; fgets(BUF, LEN, in) ; ++num)
		vec.push_back(std::make_pair(BUF, num));

	return io_status::success;
}


io_status t1_solve (const char *f_in, const char *f_out, int& r)
{
	io_status ret;
	vec_t vec;

	ret = read_file(f_in, vec);
	if (ret != io_status::success)
		return ret;
	
	return io_status::success;
}
