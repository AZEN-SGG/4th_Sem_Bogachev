#include "record.h"

template <>
std::size_t record::get_hash <ordering::group> () { return static_cast<std::size_t>(group); }

template <>
int record::cmp <ordering::name> (const record& x) { return cmp_word(*this, x); }

template <>
int record::cmp <ordering::group> (const record& x) { return cmp_group(*this, x); }

template <>
int record::cmp <ordering::phone> (const record& x) { return cmp_phone(*this, x); }
