#include "student.h"
#include "launch.h"

#include <cstdio>

int main(int argc, char *argv[])
{
	int r;

	if (
		!(argc == 4
		&& sscanf(argv[1], "%d", &r) == 1
	)) {
		printf("Usage %s r \"string\" filename\n", argv[0]);
		return 1;
	}

	return launch<student>(argv[0], argv[3], argv[2], r);
}
