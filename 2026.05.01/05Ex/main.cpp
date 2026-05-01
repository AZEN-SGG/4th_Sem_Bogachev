#include "io_status.h"
#include "solve.h"

#include <cstdio>
#include <ctime>

int main(int argc, char *argv[])
{
	io_status ret;
	int res = 0, task = 5;
	double t;

	if (
		!(argc == 5
	)) {
		printf("Usage %s f_a f_b f_out t\n", argv[0]);
		return 1;
	}

	t = clock();
	ret = t5_solve(argv[1], argv[2], argv[3], argv[4], res);
	t = (clock() - t) / CLOCKS_PER_SEC;

	do {
		switch (ret)
		{
			case io_status::success:
				continue;
			case io_status::open:
				fprintf (stderr, "Error: Cannot open file\n");
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
		}

		return 2;
	} while (0);
	
	fprintf(stdout, "%s : Task = %d Result = %d Elapsed = %.2f\n", argv[0], task, res, t);

	return 0;
}
