#include "database_client.h"
#include "net_status.h"
#include <cstring>

net_status database_client::query_handler (int fd)
{
	net_status ret = net_status::success;
	char buf[LEN] = {};
	FILE *f_in = stdin;
	int is_quit = 0;

	while (fgets(buf, LEN, f_in))
	{
		char *saveptr = nullptr,
			 *cmd = buf;

		while ((cmd = strtok_r(cmd, ";\n", &saveptr)) != nullptr)
		{
			if (strncmp(cmd, "quit", 4) == 0 ||
				strncmp(cmd, "stop", 4) == 0)
				is_quit = 1;

			ret = write_server(fd, cmd, LEN);
			if (ret != net_status::success)
				return ret;

			ret = read_server(fd, stdout);
			if (ret != net_status::success)
				return ret;

			if (is_quit)
				return net_status::success;

			cmd = nullptr;
		}
	}

	if (feof(f_in) != 0)
		ret = net_status::success;
	else
	{
		perror("Client: read stdin");
		ret = net_status::read;
	}

	return ret;
}
