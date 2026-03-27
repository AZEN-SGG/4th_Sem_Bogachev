#include "record.h"

template <>
int record::cmp <char *> (const record& x) { return cmp_word(*this, x); }

