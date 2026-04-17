#ifndef SOLVE_H
#define SOLVE_H

#include "net_status.h"
#include "database_client.h"

#include <new>
#include <memory>
#include <cstdio>
#include <cstring>

net_status solve (char *addr, int port);

#endif // SOLVE_H
