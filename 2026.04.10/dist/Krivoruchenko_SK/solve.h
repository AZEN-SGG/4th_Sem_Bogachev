#ifndef SOLVE_H
#define SOLVE_H

#include "io_status.h"
#include "database.h"
#include "record.h"
#include "command_type.h"
#include "command.h"

#include <new>
#include <memory>
#include <cstdio>
#include <cstring>

io_status solve (char *filename, int *r, double *time);

#endif // SOLVE_H
