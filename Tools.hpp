#ifndef TOOLS_HPP
# define TOOLS_HPP
# include <iostream>
# include <string>

namespace ft
{
	template <class Container>
	void  split(const std::string & str, Container & cont, char delim = ' ')
	{
		std::size_t current, previous= 0;
			
		current = str.find(delim);
		while (current != std::string::npos)
		{
			cont.push_back(str.substr(previous, current - previous));
			previous = current + 1;
			current = str.find(delim, previous);
		}
		cont.push_back(str.substr(previous, current - previous));
	}

	template <class Container>
	bool	checkMode(std::string const & str, Container & cont)
	{
		typename Container::iterator it;

		for (it = cont.begin(); it != cont.end(); it++)
		{
			if (*it == str)
				return true;
		}
		return false;
	}

	int	checkString(std::string str);
}

#endif