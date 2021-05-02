#include "Channel.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Channel::Channel(Client *cli, std::string const &name)
{
    clients.push_back(cli);
    name_ = name;
}

Channel::Channel(std::string const &name)
{
    name_ = name;
}



/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Channel::~Channel()
{
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/


/*
** --------------------------------- METHODS ----------------------------------
*/

void        	Channel::setName(std::string const &name)
{
    name_ = name;
}

void        	Channel::setTopic(std::string const &topic)
{
    topic_ = topic;
}

void        	Channel::addClient(Client *cli)
{
    clients.push_back(cli);
}

std::string		Channel::getModes()
{
	std::vector<std::string>::iterator	it = mode_.begin();
	std::string 						ret;

	for ( ; it != mode_.end(); it++)
		ret.append(*it);
	return ret;
}

std::string 	Channel::getMemberModes(std::string const &nick)
{
	std::map<std::string, std::string>::iterator it;
	if ((it = clientsmode.find(nick)) != clientsmode.end())
		return it->second;
	return nullptr;
}

void			Channel::setMemberModes(std::string const &modes, char operation, std::string &ret)
{
	if (operation == '+')
	{
		for (size_t i = 0; i < modes.length(); i++)
		{
			if (ret.find(modes[i]) == std::string::npos)
				ret.append(std::string(1, modes[i]));
		}
	}
	else if (operation == '-')
	{
		for (size_t i = 0; i < modes.length(); i++)
		{
			for (size_t j = 0; j < ret.length(); j++)
			{
				if (modes[i] == ret[j])
					ret.erase(j);
			}
		}
	}
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */