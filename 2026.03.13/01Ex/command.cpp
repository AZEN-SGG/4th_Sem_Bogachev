#include "command.h"


void command::print (FILE *fp = stdout) const
		{
			condition cond = condition::none;
			if (c_group != condition::none)
			{
				fprintf(fp, "%d", group);
				cond = c_group;
			} else if (c_phone != condition::none)
			{
				fprintf(fp, "%d", phone);
				cond = c_phone;
			} else if (c_name != condition::none)
			{
				fprintf(fp, "%s", word.get());
				cond = c_name;
			} else
				fprintf(fp, "none");

			switch (cond)
			{
				case condition::none:
					fprintf(fp, " none\n\n");
					break;
				case condition::lt:
					fprintf(fp, " <\n\n");
					break;
				case condition::gt:
					fprintf(fp, " >\n\n");
					break;
				case condition::le:
					fprintf(fp, " <=\n\n");
					break;
				case condition::ge:
					fprintf(fp, " >=\n\n");
					break;
				case condition::eq:
					fprintf(fp, " =\n\n");
					break;
				case condition::ne:
					fprintf(fp, " <>\n\n");
					break;
				case condition::like:
					fprintf(fp, " like\n\n");
					break;
				case condition::nlike:
					fprintf(fp, " not like\n\n");
					break;
			}
		}
