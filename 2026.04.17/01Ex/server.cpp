#include <cstdio>
#include <cstdlib>

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
#define BUFLEN	512

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
	// Далее передаём значение
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
			}
		}
	}
}

