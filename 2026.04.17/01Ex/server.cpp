#include "server.h"
#include "io_status.h"
#include <cstddef>
#include <ctime>

int server::setup (const int p, const int t)
{
	int err, opt = 1;

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
			if (FD_ISSET(i, &read_set))
			{
				if (i == sock)
				{
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
				} else
				{
					err = readFromClient(i, buf);
					if (err < 0)
					{
						// Либо ошибка, либо клиент оборвал соединение
						close(i);
						FD_CLR(i, &active_set);
					} else
					{
						FILE *out = fdopen(i, "r");
						io_status ret = query_handler(buf, out);
						if (ret != io_status::success)
						{

							return ret;
						}
					}
				}
			} else if (FD_ISSET(i, &active_set) &&
					(((clock() - fd_time[i]) / CLOCKS_PER_SEC) > timeout))
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

#include <cctype>
#include <cstdio>
#include <cstdlib>

// read - write
#include <sys/select.h>
#include <unistd.h>
#include <sys/types.h>

// fd_set, socklen_t
#include <sys/socket.h>

// sockaddr_in
#include <netinet/in.h>

// inet_ntoa
#include <arpa/inet.h>

#include <netdb.h>

// Больше 1024, т.к. до - они зарезервированы
#define PORT	5555
#define BUFLEN	1234

// fd - файловый дескриптор
int readFromClient (int fd, char *buf);
int writeToClient (int fd, char *buf);

int main (void)
{
	int i, err, opt = 1;
	int sock, new_sock;

	// Множество файловых дескрипторов
	fd_set active_set, read_set;

	// Переменная с адресом клиента и сервера
	// Содержит порт, адрес и тип семейства?
	struct sockaddr_in addr;
	struct sockaddr_in client;

	// AF_INET - Обозначает тип адреса IPv4 - т.е. формата 0.0.0.0 - 255.255.255.255
	
	char buf[BUFLEN];
	socklen_t size;



	// Создаём TCP сокет для приёма запросов на соединение
	// socket - создаёт сокет и возвращает число - файловый дескриптор (идентификатор сокета в данном процессе)
	// Вернёт -1 в случае неудачи
	// Первый арг. - семейство адресов в которых будет жить сокет, PF_INET и AF_INET в большинстве случаев одно и то же
	// Второй арг. - тип обмена, в данном - потоковый, нет раздаления на сообщения - поток данных
	// Третий арг. - конкретный протокол, 0 - автоматически, скорее всего TCP
	sock = socket (PF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		// Сама завершает вывод
		perror("Server: cannot create socket");
		// Немедленно завершает всю программу
		exit(EXIT_FAILURE);
	}

	// Установить опцию для сокета, 0 - ок, -1 - ошибка
	// Передаём дескриптор на сокет
	// SOL_SOCKET - "раздел" настроек, может быть например IPPROTO_TCP
	// SO_REUSEADDR - имя опции, разрешает повторное использование адреса и порта
	// Далее передаём значение, opt = 1 -> включаем опцию
	// Передаём размер нашего типа (сколько нужно прочитать)
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

	// Настраиваем структуру адресов
	// Семейство адресов (тут - IPv4)
	addr.sin_family = AF_INET;
	// Задаём порт переведя его в верный формат
	// sin_port - порт на котором приложение будет слушать
	// htons - host to network short - short - unsigned short - 2 байта
	addr.sin_port = htons(PORT);
	// htonl - long - 4 байта
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	// Связываем порт в системе с сокетом, через файловый дескриптор
	err = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
	if (err < 0)
	{
		perror("Server: listen queue failure");
		exit(EXIT_FAILURE);
	}

	// Зануляет множество
	FD_ZERO(&active_set);
	// Добавляем дескпритор в множество
	FD_SET(sock, &active_set);

	// Цикл проверки состояния сокетов
	while (1)
	{
		// Проверка на появление данных в каком-либо сокете
		// Ждём до фактического появления данных
		read_set = active_set;

		// 1. число до которого будут проверяться дескрипторы, тут FD_SETSIZE - максимально возможный номер дескриптора
		// 2. Набор дескрипторов для чтения
		// 3. Набор дескрипторов для записи
		// 4. Условия на дескриптор (хз)
		// 5. Таймаут (struct timeval)
		// Важно: меняется набор, остаются лишь готовые к чтению!
		// Возвращает: < 0 - ошибка, = 0 - не нашлось (если есть таймаут), > 0 - число готовых дескрипторов
		if (select(FD_SETSIZE, &read_set, NULL, NULL, NULL) < 0)
		{
			perror("Server: select failure");
			exit(EXIT_FAILURE);
		}

		// Появились данные
		for (i = 0 ; i < FD_SETSIZE ; ++i)
		{
			if (FD_ISSET(i, &read_set))
			{
				if (i == sock)
				{
					// Запрос на новое соединение
					// Нынешний файловый дескриптор не имеет активного подключения, при подключении выделяется отдельный фд для данного подключения
					size = sizeof(client);
					// Принимает запрос от данного слушающего сокета, в client записываем данные о клиенте
					// В new_sock записываем файловый дескриптор данного подключения
					new_sock = accept(sock, (struct sockaddr *)&client, &size);
					if (new_sock < 0)
					{
						perror("accept");
						exit(EXIT_FAILURE);
					}

					// Выводим:
					// inet_ntoa - приводит ip клиента к типовому: 255.255.255.255
					// ntohs - приводит порт к привычному виду
					fprintf(stdout, "Server: connect from host %s, port %d.\n", inet_ntoa(client.sin_addr), (unsigned int)ntohs(client.sin_port));
					FD_SET(new_sock, &active_set);
				} else
				{
					// Пришли данные в уже существующем соединении
					err = readFromClient(i, buf);
					if (err < 0)
					{
						// Произошла ошибка, либо конец подключения
						close(i);
						FD_CLR(i, &active_set);
					} else
					{
						// Данные прочитаны
						writeToClient(i, buf);
						close(i);
						FD_CLR(i, &active_set);
						if (!strcmp(buf, "STOP"))
						{
							close(sock);
							return 0;
						}
					}
				}
			}
		}
	}
}

int readFromClient (int fd, char *buf)
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

	if (len > BUFLEN)
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

int writeToClient (int fd, char *buf)
{
	int nbytes, len, i;
	unsigned char *s;

	for (s = (unsigned char *)buf, len = 0 ; *s ; s++, len++)
		*s = toupper(*s);
}
