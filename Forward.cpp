#include "Server.hpp"

int     Server::forward_nick(std::string message, Client *cli)
{
    std::vector<std::string>			parser;
	std::vector<Client*>::iterator		it;
	std::string							aux;
    std::string                 		realname;
    Client                      		*newclient;
    std::string                 		modes;
	std::string							origin;
    char                        		operation;

    ft::split(message, parser, ':');
    realname = *(parser.end() - 1);

    parser.clear();
    ft::split(message, parser);
	origin = parser[0];
	origin.erase(0, 1);

	for (it = servers.begin(); it != servers.end(); it++)
	{
		if (origin == (*it)->servername_)
		{
			if (findUserInServer(parser[2]))
			{
				newclient = getClientByNick(parser[2]);
				aux = replies.nickcollisionReply(parser[2]);
				sendAll(ssl, newclient->socketfd, aux.c_str(), aux.length());
				deleteClient(newclient->socketfd);
				return 1;
			}
			newclient = new Client();
			newclient->setNickname(parser[2]);
			newclient->setHopcount(parser[3]);
			newclient->setUsername(parser[4]);
			newclient->setHost(parser[5]);
			newclient->setToken(parser[6]);
			operation = parser[7][0];
			modes = parser[7];
			modes.erase(0, 1);
			newclient->setMode(modes, operation, newclient->mode_);
			newclient->setRealname(realname);
			newclient->setServername(parser[5]);
			clients.push_back(newclient);
			for (it = servers.begin(); it != servers.end(); it++)
			{
				if ((*it)->hopcount_ == 1 && (*it)->servername_ != cli->servername_)
				{
					aux = replies.sendallnicksReply(newclient);
					sendAll(ssl, (*it)->socketfd, aux.c_str(), aux.length());
				}
			}
			return 0;
		}
	}
	newclient = getClientByNick(origin);
	aux.clear();
	aux = replies.changeNickname(newclient->nick_, newclient->user_, newclient->host_, parser[2]);
	channelBroadcasting(newclient, aux);
	newclient->setNickname(parser[2]);
    
    return 0;
}

int			Server::forward_privmsg(std::string message, Client *cli)
{
	Client						*newclient;
	Client						*tosend;
	Channel						*newchannel;
	std::string					aux;
	std::vector<std::string>	parser;
	std::string					final_msg;
	std::string					origin;
	std::vector<Client*>::iterator	it;

	ft::split(message, parser, ':');
	final_msg = *(parser.end() - 1);

	parser.clear();
	ft::split(message, parser);
	origin = parser[0];
	origin.erase(0, 1);
	if ((tosend = getClientByNick(parser[2])))
	{
		if (tosend->socketfd != 0)
		{
			newclient = getClientByNick(origin);
			aux = replies.privmsgReply(newclient, "PRIVMSG", tosend->nick_, final_msg);
			sendAll(ssl, tosend->socketfd, aux.c_str(), aux.length());
		}
		else
		{
			message.append("\r\n");
			for (it = servers.begin(); it != servers.end(); it++)
			{
				if ((*it)->hopcount_ == 1 && (*it)->socketfd != cli->socketfd)
					sendAll((*it)->ssl, (*it)->socketfd, message.c_str(), message.length());
			}
		}
	}
	else if ((newchannel = channelExist(parser[2])))
	{
		newclient = getClientByNick(origin);
		aux = replies.privmsgReply(newclient, "PRIVMSG", parser[2], final_msg);
		broadcastRest(newchannel->clients, newclient->socketfd, aux);
		message.append("\r\n");
		for (it = servers.begin(); it != servers.end(); it++)
		{
			if ((*it)->hopcount_ == 1 && (*it)->socketfd != cli->socketfd)
				sendAll((*it)->ssl, (*it)->socketfd, message.c_str(), message.length());
		}
	}
	return 0;
}

int		Server::forward_join(std::string message, Client *cli)
{
	std::vector<std::string>					parser;
	std::string									origin;
	std::string									channel_name;
	std::string									modes;
	std::map<std::string, Channel*>::iterator	it;
	Channel										*newchannel;
	Client										*newclient;
	std::string									aux;
	std::vector<Client*>::iterator				vectorit;
	std::string									forward_msg;

	forward_msg = message;
	forward_msg.append("\r\n");

	ft::split(message, parser, '\a');
	if (parser.size() > 1 && parser[1].size() != 0)
	{
		modes = parser[1];
		message.erase(message.find('\a'), (modes.length() + 1));
	}
	parser.clear();
	ft::split(message, parser);
	origin = parser[0];
	origin.erase(0, 1);
	channel_name = parser[2];
	newclient = getClientByNick(origin);

	if ((it = channels.find(channel_name)) == channels.end())
	{
		newchannel = new Channel(newclient, channel_name);
		std::pair<std::string, Channel*> pair;
		pair = std::make_pair(channel_name, newchannel);
		channels.insert(pair);
		std::pair<std::string, std::string> strpair;
		strpair = std::make_pair(newclient->nick_, modes);
		newchannel->clientsmode.insert(strpair);
		for (vectorit = servers.begin(); vectorit != servers.end(); vectorit++)
		{
			if ((*vectorit)->hopcount_ == 1 && (*vectorit)->socketfd != cli->socketfd)
				sendAll((*vectorit)->ssl, (*vectorit)->socketfd, forward_msg.c_str(), forward_msg.length());
		}
		return 0;
	}
	it->second->addClient(newclient);
	it->second->clientsmode[newclient->nick_];
	aux = replies.joinReply(newclient, it->second->name_);
	broadcastRest(it->second->clients, newclient->socketfd, aux);
	message.append("\r\n");
	for (vectorit = servers.begin(); vectorit != servers.end(); vectorit++)
	{
		if ((*vectorit)->hopcount_ == 1 && (*vectorit)->socketfd != cli->socketfd)
			sendAll((*vectorit)->ssl, (*vectorit)->socketfd, forward_msg.c_str(), forward_msg.length());
	}
	return 0;
}

int			Server::forward_quit(std::string message, Client *cli)
{
	std::vector<std::string>					parser;
	std::string									aux;
	std::string									msg;
	std::string									origin;
	std::vector<Client*>::iterator				cliit;
	std::map<std::string, Channel*>::iterator	it;
	Client										*newclient;
	
	ft::split(message, parser, ':');
	if (parser.size() > 1)
		msg = *(parser.end() - 1);
	else
		msg = " ";

	parser.clear();
	ft::split(message, parser);
	origin = parser[0];
	origin.erase(0, 1);
	newclient = getClientByNick(origin);
	aux = replies.clientQuitFromChannel(newclient->nick_, newclient->user_, newclient->host_, msg);
	it = channels.begin();
	for ( ; it != channels.end(); it++)
	{
		if (findUserInChannel(newclient->nick_, it->second))
		{
			deleteClientFromChannel(newclient->nick_, it->second->clients);
			broadcastRest(it->second->clients, cli->socketfd, aux);
		}
	}
	deleteClient(origin);
	message.append("\r\n");
	for (cliit = servers.begin(); cliit != servers.end(); cliit++)
	{
		if ((*cliit)->hopcount_ == 1 && (*cliit)->socketfd != cli->socketfd)
			sendAll((*cliit)->ssl, (*cliit)->socketfd, message.c_str(), message.length());
	}
	return 0;
}

int		Server::forward_part(std::string message, Client *cli)
{
	std::string							aux;
	std::vector<std::string>			parser;
	std::string							origin;
	std::string							info;
	std::string							channame;
	Client								*newclient;
	Channel								*newchannel;
	std::vector<Client*>::iterator		cliit;

	ft::split(message, parser, ':');
	if (parser.size() > 1 && parser[1].size() > 0)
		info = *(parser.end() - 1);

	parser.clear();
	ft::split(message, parser);
	origin = parser[0];
	origin.erase(0, 1);
	channame = parser[2];
	newclient = getClientByNick(origin);
	newchannel = channelExist(channame);

	aux = replies.partReply(newclient, channame);
	broadcastRest(newchannel->clients, cli->socketfd, aux);

	for (cliit = newchannel->clients.begin(); cliit != newchannel->clients.end(); cliit++)
	{
		if ((*cliit)->nick_ == origin)
		{
			newchannel->clients.erase(cliit);
			break ;
		}
	}
	if (newchannel->clients.size() == 0)
		channels.erase(newchannel->name_);
	
	message.append("\r\n");
	for (cliit = servers.begin(); cliit != servers.end(); cliit++)
	{
		if ((*cliit)->hopcount_ == 1 && (*cliit)->socketfd != cli->socketfd)
			sendAll((*cliit)->ssl, (*cliit)->socketfd, message.c_str(), message.length());
	}
	return 0;
}