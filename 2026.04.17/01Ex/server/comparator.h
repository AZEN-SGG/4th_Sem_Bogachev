#ifndef COMPARATOR_H
#define COMPARATOR_H

template <typename T>
struct comparator
{
	using cmp_t = int (*)(const T&, const T&);

	cmp_t first;
	cmp_t second;
	cmp_t third;

	int operator() (const T& x, const T& y)
	{
		int cmp = 0;

		if ((cmp = first(x, y)) != 0)
			return cmp;

		if ((cmp = second(x, y)) != 0)
			return cmp;

		return third(x, y);
	}
};

#endif // COMPARATOR_H
