#include "database_client.h"
#include "net_status.h"

net_status database_client::query_handler (int fd)
{
	net_status ret = net_status::success;
	char buf[LEN] = {};
	FILE *f_in = stdin;

	while (fgets(buf, LEN, f_in))
	{
		char *saveptr = nullptr,
			 *cmd = buf;

		while ((cmd = strtok_r(cmd, ";\n", &saveptr)) != nullptr)
		{
			ret = write_server(fd, cmd, LEN);
			if (ret != net_status::success)
				return ret;

			ret = read_server(fd, stdout);
			if (ret != net_status::success)
				return ret;

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
