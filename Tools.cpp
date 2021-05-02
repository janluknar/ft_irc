#include "Tools.hpp"

namespace ft
{
	int	checkString(std::string str)
	{
		for (unsigned long i = 0; i < str.length(); i++)
		{
			if (!isalnum(str[i]) && str[i] != '-' && str[i] != '\\' && str[i] != '_' && str[i] != '[' && str[i] != ']'
			&& str[i] != '{' && str[i] != '}' && str[i] != '`' && str[i] != '|')
				return 1;
		}
		return 0;
	}
}