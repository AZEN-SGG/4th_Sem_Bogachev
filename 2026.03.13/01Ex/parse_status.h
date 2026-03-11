#ifndef PARSE_STATUS_H
#define PARSE_STATUS_H

enum class parse_status
{
	none,
	success,
	end_of_string,
	name,
	phone,
	group,
	output_c,
	nlike

};

#endif // PARSE_STATUS_H
