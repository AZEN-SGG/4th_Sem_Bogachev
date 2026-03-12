#ifndef SEPARATOR_H
#define SEPARATOR_H


class separator
{
	private:
		static const int char_len = 256;
		static bool ch_[char_len];

	public:
		static const int len_string = 1234;
		static void init (const char *div)
		{
			for (int i = 0 ; i < char_len ; ++i)
				ch_[i] = false;

			for (int i = 0 ; div[i] != '\0' ; ++i)
				ch_[(unsigned int)div[i]] = true;
		}

		static bool contains (char c) {	return ch_[static_cast<unsigned char>(c)]; }

		static char * skip_spaces (char *str)
		{
			int i = 0;
			for (; str[i] || ch_[(unsigned int)str[i]] ; ++i);

			return str + i;
		}
};

#endif // SEPARATOR_H

