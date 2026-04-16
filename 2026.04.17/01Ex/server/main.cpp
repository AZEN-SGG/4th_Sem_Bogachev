#include <cstdio>
#include <ctime>

#include "io_status.h"
#include "ordering.h"
#include "separator.h"

#include "record.h"
#include "fast_search.h"
#include "solve.h"

template<>
unsigned int list2<record>::r = -1;

template<>
const int index_array<record, ordering::group>::max_size = 1000;

bool separator::ch_[separator::char_len] = {};

int main(int argc, char * argv[])
{
	int port, res = 0;
	io_status ret;
	double time;
	
	if (!(argc == 3 &&
		sscanf(argv[2], "%d", &port) == 1)
	) {
		fprintf(stderr, "Usage: %s filename port\n", argv[0]);
		return 1;
	}

	separator::init(" \t\n");

	ret = solve(argv[0], argv[1], port, &res, &time);

	do {
		switch (ret)
		{
			case io_status::success:
				continue;
			case io_status::open:
				fprintf (stderr, "Error: Cannot open file\n");
				break;
			case io_status::read:
				fprintf (stderr, "Error: While reading of file %s\n", argv[1]);
				break;
			case io_status::format:
				fprintf (stderr, "Error: Wrong format of file %s\n", argv[1]);
				break;
			case io_status::eof:
				fprintf (stderr, "Error: End of file %s\n", argv[1]);
				break;
			case io_status::memory:
				fprintf (stderr, "Error: MEMORY\n");
				break;
			case io_status::create:
				fprintf (stderr, "Error: Create, how is it possible?!\n"); // it is impossible...
				break;
			case io_status::network:
				fprintf(stderr, "Error: Something with server!\n");
				break;
			case io_status::quit:
				fprintf(stdout, "Exiting programm...\n");
				break;
		}

		return 3;
	} while (0);

	fprintf(stdout, "%s : Result = %d Elapsed = %.2f\n", argv[0], res, time);

	return 0;
}
