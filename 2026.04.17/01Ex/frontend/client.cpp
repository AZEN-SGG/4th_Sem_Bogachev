#include "client.h"
#include <cstring>


net_status client::setup ()
{
	// Создаем TCP сокет.
	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		perror("Client: socket was not created");
		return net_status::socket;
	}

	return net_status::success;
}

net_status client::run (char *addr, int port)
{
	net_status ret;
	struct sockaddr_in server_addr;
	struct hostent *hostinfo;

	if (sock < 0)
	{
		perror("Client: socket was not init!");
		return net_status::socket;
	}

	// Получаем информацию о сервере
	hostinfo = gethostbyname(addr);
	if (hostinfo == NULL)
	{
		fprintf(stderr, "Unknown host %s.\n", addr);
		return net_status::host;
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr = *(struct in_addr *)hostinfo->h_addr;

	// Устанавливаем соединение с сервером
	int err = connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (err < 0)
	{
		perror("Client: connect failure");
		return net_status::connect;
	}

	fprintf(stdout, "Connection is ready\n");

	ret = query_handler(sock);
	if (ret != net_status::success)
		return ret;

	return net_status::success;
}

net_status client::write_server(int fd, char *buf, int size)
{
	int nbytes, len, i;

	// Длина сообщения
	len = strlen(buf) + 1;

	// Пересылаем длину сообщения
	if (write(fd, &len, sizeof(int)) != (int)sizeof(int))
	{
		perror("Client: write length");
		return net_status::write;
	}

	// Пересылаем len байт
	for (i = 0; len > 0; i += nbytes, len -= nbytes)
	{
		nbytes = write(fd, buf + i, len);
		if (nbytes < 0)
		{
			perror("Client: can't write");
			return net_status::write;
		} else if (nbytes == 0)
		{
			perror("Client: write truncated");
			return net_status::write;
		}
	}

	return net_status::success;
}

net_status client::read_server(int fd, FILE *fp)
{
	static const int BUF_SIZE = 8192;

	static const char end_marker[] = "\nEND\n";
	const int end_len = 5;
	const int tail_len = end_len - 1;
	char tail[tail_len];
	int tail_size = 0;

	char buf[BUF_SIZE];

	while (1)
	{
		int nbytes = read(fd, buf, BUF_SIZE);

		if (nbytes < 0)
		{
			perror("Client: read");
			return net_status::read;
		}

		if (nbytes == 0)
		{
			// Соединение закрыто
			if (tail_size > 0)
			{
				// Был хвост - печатаем!
				if (fwrite(tail, 1, tail_size, fp) != (size_t)tail_size)
				{
					perror("Client: fwrite");
					return net_status::write;
				}
			}
			break;
		}

		if (nbytes >= end_len &&
			memcmp(buf + nbytes - end_len, end_marker, end_len) == 0)
		{
			// Прочитанный блок кончается на маркер
			if (tail_size > 0)
			{
				// Если был хвост с прошлого раза - то он не маркер, значит печатаем
				if (fwrite(tail, 1, tail_size, fp) != (size_t)tail_size)
				{
					perror("Client: fwrite");
					return net_status::write;
				}
			}

			if (nbytes > end_len)
			{
				// Печатаем остальное
				if (fwrite(buf, 1, nbytes - end_len, fp) != (size_t)(nbytes - end_len))
				{
					perror("Client: fwrite");
					return net_status::write;
				}
			}

			break;
		}

		 // Проверяем все варианты:
		 // 1 байт из tail + 4 байта из buf
		 // и т.д., лень...
		for (int k = 1; k <= tail_size; k++)
		{
			// k взяли из хвоста
			int need_from_buf = end_len - k;

			if (nbytes < need_from_buf)
				// Недостаточно в прочитанном - точно не конец
				continue;

			if (memcmp(tail + tail_size - k, end_marker, k) == 0 &&
				memcmp(buf, end_marker + k, need_from_buf) == 0)
			{
				// Маркер найден, о как!
				int tail_data_len = tail_size - k;

				if (tail_data_len > 0)
				{
					if (fwrite(tail, 1, tail_data_len, fp) != (size_t)tail_data_len)
					{
						perror("Client: fwrite");
						return net_status::write;
					}
				}

				return net_status::success;
			}
		}

		// Маркер не найден

		if (tail_size > 0)
		{
			// Печатаем прошлый хвост
			if (fwrite(tail, 1, tail_size, fp) != (size_t)tail_size)
			{
				perror("Client: fwrite");
				return net_status::write;
			}
		}

		int safe_len = nbytes - tail_len;
		if (safe_len > 0)
		{
			// Печатаем новые данные, до нового хвоста
			if (fwrite(buf, 1, safe_len, fp) != (size_t)safe_len)
			{
				perror("Client: fwrite");
				return net_status::write;
			}
		}

		memcpy(tail, buf + safe_len, tail_len);
		tail_size = tail_len;
	}

	return net_status::success;
}

