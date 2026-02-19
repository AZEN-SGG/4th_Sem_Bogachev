#include "launch.h"

template <typename T>
int launch (char *path, char *filename, const char *s, int r)
{
	avl_tree<T> *olha = new avl_tree<T>;
    if (olha == nullptr)
    {
		fprintf (stderr, "Error: MEMORY -> tree is nullptr\n");
        return 2;
    }

	io_status ret = olha->read_file(filename);

	do {
		switch (ret)
		{
			case io_status::success:
				continue;
			case io_status::open:
				fprintf (stderr, "Error: Cannot open %s\n", filename);
				break;
			case io_status::format:
				fprintf (stderr, "Error: Wrong format of file %s\n", filename);
				break;
			case io_status::eof:
				fprintf (stderr, "Error: End of file %s\n", filename);
				break;
			case io_status::memory:
				fprintf (stderr, "Error: MEMORY\n");
				break;
			case io_status::create:
				fprintf (stderr, "Error: Create, how is it possible?!\n"); // it is impossible...
				break;
		}

		delete olha;

		return 3;
	} while (0);

	fprintf (stdout, "Original tree:\n");
	olha->print(r);

    int (avl_tree<T>::*solves[])(const char *) const = {
        &avl_tree<T>::t1_solve,
        &avl_tree<T>::t2_solve,
        &avl_tree<T>::t3_solve,
        &avl_tree<T>::t4_solve,
        &avl_tree<T>::t5_solve,
    };
    int len = sizeof(solves) / sizeof(solves[0]);

	const char * s_all = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

	double t = 0;
    int res = 0;
    for (int i = 0 ; i < len ; ++i)
    {
		// For s_all
	    t = clock();
        res = (olha->*solves[i])(s_all);
        t = (clock() - t) / CLOCKS_PER_SEC;

	    fprintf(stdout, "%s : Task = %d S = %s Result = %d Elapsed = %.2f\n", path, i + 1, s_all, res, t);

		// For s
	    t = clock();
        res = (olha->*solves[i])(s);
        t = (clock() - t) / CLOCKS_PER_SEC;

	    fprintf(stdout, "%s : Task = %d S = %s Result = %d Elapsed = %.2f\n", path, i + 1, s, res, t);
    }

	delete olha;

	return 0;
}

template int launch<student>(char *, char *, const char *, int);

