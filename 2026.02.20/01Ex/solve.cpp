#include "solve.h"

io_status solve_01 (char * f_in, char * f_out, char * s, char * t, int m, int * r)
{
	(void)f_in;
	(void)f_out;
	(void)r;

	b_tree<line> *olha = new b_tree<line>{m};
    if (olha == nullptr)
		return io_status::memory;

	io_status ret = olha->read(s, t);
	if (ret != io_status::success)
	{
		delete olha;
		return ret;
	}

	olha->print(100);

	delete olha;
	return io_status::success;
}
