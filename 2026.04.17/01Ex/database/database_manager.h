#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include "database.h"
#include "server.h"

#include "io_status.h"
#include "record.h"
#include "command.h"

#include <memory>

class database_manager : public server
{
private:
	std::unique_ptr<database<record>> db = nullptr;
public:
	database_manager () { db = std::make_unique<database<record>>(); }

	io_status read_file (char *filename, const unsigned int max_read = -1);
	io_status query_handler (char *buf, int *res, FILE *f_out = stdout);
};

#endif // DATABASE_MANAGER_H
