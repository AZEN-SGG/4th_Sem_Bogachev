#ifndef COMMAND_H
#define COMMAND_H

#include "record.h"

#include <cstdio>

class command : public record
{
	private:
		condition c_name	= condition::none;
		condition c_phone	= condition::none;
		condition c_group	= condition::none;
	public:
		command () = default;
		~command () = default;

		bool parse (const char *string)
		{

		}

		void print (FILE *fp = stdout) const
		{

		}

		bool apply (const record& x) const
		{

		}
};

#endif // COMMAND_H
