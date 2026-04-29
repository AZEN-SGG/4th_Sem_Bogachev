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


	fprintf (stdout, "Original tree:\n");
	olha->print(r);

    int (avl_tree<T>::*solves[])(const char *) const = {
        &avl_tree<T>::t1_solve,
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

