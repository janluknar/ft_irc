#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <iostream>
# include <string>
# include <vector>
# include <map>
# include "Tools.hpp"

class Server;
class Client;

class Channel
{
	friend class Server;
	friend class Reply;
	public:
		Channel(Client *cli, std::string const& name);
		Channel(std::string const& name);
		~Channel();

        void        	setName(std::string const& name);
        void        	setTopic(std::string const &topic);
        void        	addClient(Client *cli);
		//Devuelve los modos del canal
		std::string		getModes();
		//Devuelve los modos de un miembro del canal
		std::string		getMemberModes(std::string const & nick);
		//Setea los modos miembro de un usuario del canal
		void			setMemberModes(std::string const &modes, char operation, std::string &ret);

	template <class Container>
	void	channelSetMode(std::string const & mode, char operation, Container &cont)
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
        Channel();
		Channel &		operator=( Channel const & rhs );
		Channel( Channel const & src );

        std::vector<Client*>        		clients;
        std::string                 		topic_;
        std::string                 		name_;
        std::vector<std::string>			mode_;
		std::vector<std::string>			invites_;
		//		NOMBRE CLIENTE, MODOS CLIENTE
		std::map<std::string, std::string>	clientsmode;

};

# include "Server.hpp"
# include "Client.hpp"

#endif /* ********************************************************** CHANNEL_H */