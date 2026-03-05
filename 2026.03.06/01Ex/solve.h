#ifndef SOLVE_H
#define SOLVE_H

#include "record.h"
#include "command.h"
#include "list.h"
#include "io_status.h"

#include <memory>
#include <cstdio>
#include <cstring>

io_status start_db (const list2<record> *db);
io_status solve (char *filename, int *r);

#endif // SOLVE_H
