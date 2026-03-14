#ifndef VALIDATOR_H
#define VALIDATOR_H

#include "operation.h"

template <typename T, typename X>
struct validator
{
	using valid_t = bool (T::*)(X&) const;

	valid_t fname;
	valid_t fphone;
	valid_t fgroup;
	operation op;

	bool operator() (const T& obj, X& x)
	{
		bool ret = true;

		switch (op) {
			case operation::land:
				ret = ((obj.*fphone)(x) &&
						(obj.*fgroup)(x) &&
						(obj.*fname)(x));
				break;
			case operation::lor:
				ret = ((obj.*fphone)(x) ||
					  	(obj.*fgroup)(x) ||
						(obj.*fname)(x));
				break;
			case operation::none:
				ret = true;
		}

		return ret;
	}
};


#endif // VALIDATOR_H
