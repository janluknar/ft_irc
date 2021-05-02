#ifndef CLIENT_HPP
# define CLIENT_HPP

# include <iostream>
# include <string>
# include <vector>
# include <map>
# include "Tools.hpp"
# include "Channel.hpp"

class Server;

class Client
{
	friend class Server;
	friend class Reply;
	public:
		Client();
		Client(int newfd);
		Client(int newfd, SSL *ssl);
		~Client();

		int				getSocket() const;
		void			setNickname(std::string const &nick);
		void			setLogin();
		void			setUsername(std::string const &user);
		void			setHost(std::string const &host);
		void			setHopcount(std::string const &hopcount);
		void			setToken(std::string const &token);
		void			setRealname(std::string const &realname);
		void			setRol(std::string const &rol);
		void			setServername(std::string const &servername);
		void			setInfo(std::string const &info);
		std::string		getModes();
		void			setAway(std::string const &message);
		
	template <class Container>
	void	setMode(std::string const & mode, char operation, Container &cont)
	{
		typename Container::iterator it;
		typename Container::iterator ite;

		if (ft::checkMode(mode, cont) == true && operation != '-')
			return ;
		if (operation == '+')
		{
			for (size_t i = 0; i < mode.length(); i++)
				cont.push_back(std::string(1, mode[i]));
		}
		else if (operation == '-')
		{
			for (size_t i = 0; i < mode.length(); i++)
			{
				for (it = cont.begin(), ite = cont.end(); it != ite; it++)
				{
					if (*it == std::string(1, mode[i]))
						cont.erase(it);
				}
			}
		}
	}

	private:
		Client &		operator=( Client const & rhs );
		Client( Client const & src );

		int												socketfd;
		SSL												*ssl;
		char											buf[2048];
		int												readpos;
		std::string										host_;
		std::string										nick_;
		std::string										user_;
		std::string										realname_;
		std::string										servername_;
		std::string										away;
		std::vector<std::string>						mode_;
		std::vector<std::string>						channelmode_;
		std::string										rol_;
		int												hopcount_;
		int												token_;
		std::string										info_;
		std::time_t 									signontime;
		bool											is_secure;
		int												msgSendnum;
		int												bytesSendnum;
		int												msgRecnum;
		int												bytesRecnum;
};

# include "Server.hpp"

#endif /* ********************************************************** CLIENT_H */