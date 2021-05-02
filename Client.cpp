#include "Client.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Client::Client() :socketfd(0), ssl(0), readpos(0), rol_("user")
{
	bzero(buf, 2048);
	signontime = std::time(0);
	msgRecnum = 0;
	msgSendnum = 0;
	bytesRecnum = 0;
	bytesSendnum = 0;
}

Client::Client(int newfd) : socketfd(newfd), ssl(0), readpos(0), rol_("unknown"), is_secure(false)
{
	bzero(buf, 2048);
	signontime = std::time(0);
	msgRecnum = 0;
	msgSendnum = 0;
	bytesRecnum = 0;
	bytesSendnum = 0;
}

Client::Client(int newfd, SSL *ssl) : socketfd(newfd), ssl(ssl), readpos(0), rol_("unknown"), is_secure(false)
{
	bzero(buf, 2048);
	signontime = std::time(0);
	msgRecnum = 0;
	msgSendnum = 0;
	bytesRecnum = 0;
	bytesSendnum = 0;
}



/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Client::~Client()
{
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/


/*
** --------------------------------- METHODS ----------------------------------
*/

int		Client::getSocket() const
{
	return socketfd;
}

std::string		Client::getModes()
{
	std::vector<std::string>::iterator	it = mode_.begin();
	std::string 						ret;

	for ( ; it != mode_.end(); it++)
		ret.append(*it);
	return ret;
}

void	Client::setNickname(std::string const &nick)
{
	nick_ = nick;
}

void	Client::setUsername(std::string const &user)
{
	user_ = user;
}

void	Client::setRealname(std::string const &realname)
{
	realname_ = realname;
}

void	Client::setHost(std::string const &host)
{
	host_ = host;
}

void	Client::setHopcount(std::string const &hopcount)
{
	hopcount_ = std::stoi(hopcount);
}

void	Client::setToken(std::string const &token)
{
	token_ = std::stoi(token);
}

void	Client::setServername(std::string const &servername)
{
	servername_ = servername;
}

void	Client::setInfo(std::string const &info)
{
	info_ = info;
}

void	Client::setLogin()
{
	is_secure = true;
}

void	Client::setRol(std::string const &rol)
{
	rol_ = rol;
}

void	Client::setAway(std::string const &message)
{
	away = message;
}

/*
** --------------------------------- ACCESSOR ---------------------------------
*/


/* ************************************************************************** */