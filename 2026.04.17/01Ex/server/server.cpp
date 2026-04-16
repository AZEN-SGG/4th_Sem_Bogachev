#include "server.h"

int server::setup (const int p, const int t)
{
	int err, opt = 1;

	erase();

	port = p;
	timeout = t;

	sock = socket(PF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		perror("Server: cannot create socket");
		return -1;
	}

	max_sock = sock;
	setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	err = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
	if (err < 0)
	{
		perror ("Server: cannot bind socket");
		return -1;
	}

	return 0;
}

io_status server::run () 
{
	struct timeval tv;
	int new_sock;
	struct sockaddr_in client;
	socklen_t size;
	fd_set read_set;
	double fd_time[FD_SETSIZE];

	char buf[LEN];

	int err = listen(sock, 3);
	if (err < 0)
	{
		perror("Server: listen queue failure");
		return io_status::open;
	}

	FD_ZERO(&active_set);
	FD_SET(sock, &active_set);

	while (1)
	{
		tv.tv_sec = timeout; // Секунды
		tv.tv_usec = 0; // Микросекунды

		read_set = active_set;
		err = select(max_sock + 1, &read_set, NULL, NULL, &tv);
		if (err < 0)
		{
			perror("Server: select failure");
			return io_status::read;
		} else if (err == 0)
		{
			for (int i = 0 ; i < max_sock ; ++i)
				if (FD_ISSET(i, &active_set) && i == sock)
					close(i);
			
			FD_ZERO(&active_set);
			perror("Server: timeout");
			return io_status::read;
		}

		for (int i = 0 ; i < max_sock ; ++i)
		{
			// Идём по всем возможным сокетам
			if (FD_ISSET(i, &read_set))
			{
				// Если это сокет с подключением
				if (i == sock)
				{
					// Если это основной сокет подключения
					size = sizeof(client);
					new_sock = accept(sock, (struct sockaddr *)&client, &size);
					if (new_sock < 0)
					{
						perror("Server: accept failure");
						return io_status::read;
					}

					fprintf (stdout, "Server: connect from host %s, port %d.\n", inet_ntoa(client.sin_addr), (unsigned int)ntohs(client.sin_port));

					max_sock = max_sock < new_sock ? new_sock : max_sock;
					FD_SET(new_sock, &active_set);
					fd_time[i] = clock();
				} else
				{
					// Сокет для чтения
					
					// Фиксируем взаимодействие с сокетом
					fd_time[i] = clock();

					err = read_fd(i, buf);
					if (err < 0)
					{
						// Либо ошибка, либо клиент оборвал соединение
						close(i);
						FD_CLR(i, &active_set);
					} else
					{
						// Успешно прочитано
						
						// Создаём копию дескриптора для создания у него потока
						int temp_fd = dup(i);

						// Создаём поток для записи в сокет
						FILE *out = fdopen(temp_fd, "w");

						// Обрабатываем запрос
						io_status ret = query_handler(buf, out);
						if (ret != io_status::success)
						{
							for (int j = 0 ; j < max_sock ; ++j)
								if (j != sock)
									close(j);
							return ret;
						}

						// В конце вывода "\nEND\n"
						fprintf(out, "\nEND\n");
						// Закрываем поток, автоматически закрывается и дескриптор
						fclose(out);
					}
				}
			} else if (FD_ISSET(i, &active_set) &&
					(((clock() - fd_time[i]) / CLOCKS_PER_SEC) > timeout) &&
					sock != i)
			{
				// Timeout
				close(i);
				FD_CLR(i, &active_set);
				fd_time[i] = 0;
			}
		}
	}

	return io_status::success;
}

int server::read_fd (int fd, char *buf)
{
	int nbytes, len, i;

	// Получаем длину сообщения
	for (i = 0 ; i < (int)sizeof(int) ; i += nbytes)
	{
		nbytes = read(fd, &len + i, sizeof(int));
		if (nbytes <= 0)
		{
			perror("Server: read length");
			return -1;
		}
	}

	if (len > LEN)
	{
		perror("Server: too large message");
		return -1;
	}

	// TCP сам переотправляет потерянные пакеты
	for (i = 0 ; len > 0 ; i += nbytes, len -= nbytes)
	{
		nbytes = read(fd, buf + i, len);
		if (nbytes < 0)
		{
			perror("Server: read message");
			return -1;
		} else if (nbytes == 0)
		{
			perror("Server: read truncated");
			return -1;
		}
	}

	if (i == 0)
	{
		fprintf(stderr, "Server: no message\n");
		return -1;
	} else
	{
		fprintf(stdout, "Server: get %d bytes of message: %s\n", i, buf);
		return 0;
	}
}

