#include <cstdio>
#include <ctime>

#include "io_status.h"
#include "solve.h"


int main(int argc, char * argv[])
{
    int res = 0, task = 3;
	io_status ret;
    double t;
	
    if (!(argc == 6)
	) {
        fprintf(stderr, "Usage: %s f_in f_out s t x\n", argv[0]);
        return 1;
    }

    t = clock();
	ret = solve_03(argv[1], argv[2], argv[3], argv[5], argv[4], &res);
    t = (clock() - t) / CLOCKS_PER_SEC;

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
		}

		return 3;
	} while (0);

	fprintf(stdout, "%s : Task = %d Result = %d Elapsed = %.2f\n", argv[0], task, res, t);
    return 0;
}
