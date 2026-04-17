#ifndef IO_STATUS_H
#define IO_STATUS_H

#define LEN 1234

enum class io_status
{
	success,
	eof,
	format,
	memory,
	open,
	read,
	create,
	quit,
	stop,
	network,
};

#endif // io_status_h
