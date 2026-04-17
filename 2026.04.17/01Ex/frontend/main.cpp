#include "net_status.h"
#include "solve.h"

int main(int argc, char * argv[])
{
	int port;
	net_status ret;
	
	if (!(argc == 3 &&
		sscanf(argv[2], "%d", &port) == 1)
	) {
		fprintf(stderr, "Usage: %s address port\n", argv[0]);
		return 1;
	}

	ret = solve(argv[1], port);

	do {
		switch (ret)
		{
			case net_status::success:
				continue;
			case net_status::host:
				fprintf (stderr, "Error: Can't find host\n");
				break;
			case net_status::connect:
				fprintf (stderr, "Error: Can't connect with host\n");
				break;
			case net_status::read:
				fprintf (stderr, "Error: read\n");
				break;
			case net_status::write:
				fprintf (stderr, "Error: write\n");
				break;
			case net_status::memory:
				fprintf (stderr, "Error: MEMORY\n");
				break;
			case net_status::socket:
				fprintf (stderr, "Error: Can't create socket\n");
				break;
		}

		return 3;
	} while (0);

	return 0;
}
