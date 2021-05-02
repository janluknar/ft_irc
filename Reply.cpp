#include "Reply.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Reply::Reply()
{
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Reply::~Reply()
{
}

/*
** --------------------------------- OVERLOAD ---------------------------------
*/

/*
** --------------------------------- METHODS ----------------------------------
*/

void			Reply::init(std::string const &hostname_)
{
	hostname.append(":");
	hostname.append(hostname_);
}

std::string		Reply::noNickname()
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" 431 * :No nickname given\r\n");
	return ret;
}

std::string		Reply::erroneusNickname(std::string const & nick)
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" 432 * ");
	ret.append(nick);
	ret.append(" :Erroneus nickname\r\n");
	return ret;
}

std::string		Reply::nickAlreadyInUse(std::string const & nick)
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" 433 * ");
	ret.append(nick);
	ret.append(" :Nickname is already in use\r\n");
	return ret;
}

std::string		Reply::needMoreParams(std::string const & command)
{
	std::string ret;

	ret.append(hostname);
	ret.append(" 461 * ");
	ret.append(command);
	ret.append(" :Not enough parameters\r\n");
	return ret;
}

std::string		Reply::alreadyRegistered()
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" 462 * :Unauthorized command (already registered)\r\n");
	return ret;
}

std::string		Reply::notRegistered()
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" 451 * :You have not registered\r\n");
	return ret;
}

std::string		Reply::changeNickname(std::string const &oldnick, std::string const &username,
				std::string const &host, std::string const &newnick)
{
	std::string	ret;

	ret.append(":");
	ret.append(oldnick);
	ret.append("!~");
	ret.append(username);
	ret.append("@");
	ret.append(host);
	ret.append(" NICK :");
	ret.append(newnick);
	ret.append("\r\n");
	return ret;
}

std::string		Reply::welcomeReply(std::string const & nick, std::string const &version,
						std::time_t t, std::string const &usermodes, Client *cli)
{
	std::string ret;

	ret.append(hostname);
	ret.append(" 001 ");
	ret.append(nick);
	ret.append(" :Welcome to the Internet Relay Chat Network ");
	ret.append(nick);
	ret.append("!");
	ret.append(cli->user_);
	ret.append("@");
	ret.append(cli->host_);
	ret.append("\n");
	ret.append(hostname);
	ret.append(" 002 ");
	ret.append(nick);
	ret.append(" :Your host is ");
	ret.append(hostname);
	ret.append(" running version ");
	ret.append(version);
	ret.append("\n");
	ret.append(hostname);
	ret.append(" 003 ");
	ret.append(nick);
	ret.append(" :This server was created ");
	ret.append(ctime(&t));
	ret.append(hostname);
	ret.append(" 004 ");
	ret.append(nick);
	ret.append(" ");
	ret.append(hostname);
	ret.append(" ");
	ret.append(version);
	ret.append(" ");
	ret.append(usermodes);
	ret.append("\r\n");
	return ret;
}

std::string		Reply::motdStart(std::string const &nick)
{
	std::string ret;

	ret.append(hostname);
	ret.append(" 375 ");
	ret.append(nick);
	ret.append(" :- ");
	ret.append(hostname);
	ret.append(" Message of the Day -");
	ret.append("\r\n");
	return ret;
}

std::string		Reply::motdEnd(std::string const &nick)
{
	std::string ret;

	ret.append(hostname);
	ret.append(" 376 ");
	ret.append(nick);
	ret.append(" :End of /MOTD command.");
	ret.append("\r\n");
	return ret;
}

std::string		Reply::motd(std::string const &nick, std::string const &line)
{
	std::string ret;

	ret.append(hostname);
	ret.append(" 372 ");
	ret.append(nick);
	ret.append(" :- ");
	ret.append(line);
	ret.append("\r\n");

	return ret;
}

std::string		Reply::clientQuit(std::string const &nick, std::string const &username,
				std::string const &host, std::string const &msg)
{
	std::string	ret;

	ret.append(":");
	ret.append(nick);
	ret.append("!~");
	ret.append(username);
	ret.append("@");
	ret.append(host);
	ret.append(" QUIT :");
	ret.append(msg);
	ret.append("\n");
	ret.append("ERROR: Closing Link: ");
	ret.append(host);
	ret.append(" (");
	ret.append(msg);
	ret.append(")\r\n");
	return ret;
}

std::string		Reply::clientQuitFromChannel(std::string const &nick, std::string const &username,
				std::string const &host, std::string const &msg)
{
	std::string	ret;

	ret.append(":");
	ret.append(nick);
	ret.append("!~");
	ret.append(username);
	ret.append("@");
	ret.append(host);
	ret.append(" QUIT :");
	ret.append(msg);
	ret.append("\r\n");
	return ret;
}

std::string		Reply::noRecipient(std::string const &command, std::string const &nick)
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" 411 ");
	ret.append(nick);
	ret.append(" :No recipient given (");
	ret.append(command);
	ret.append(")\r\n");
	return ret;
}

std::string		Reply::noTextToSend(std::string const &nick)
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" 412 ");
	ret.append(nick);
	ret.append(" :No text to send");
	ret.append("\r\n");
	return ret;
}

std::string		Reply::noSuchNick(std::string const& nick, std::string const& no_nick)
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" 401 ");
	ret.append(nick);
	ret.append(" ");
	ret.append(no_nick);
	ret.append(" :No such nick/channel");
	ret.append("\r\n");
	return ret;
}

std::string		Reply::noSuchServer(std::string const& nick, std::string const& no_nick)
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" 402 ");
	ret.append(nick);
	ret.append(" ");
	ret.append(no_nick);
	ret.append(" :No such server");
	ret.append("\r\n");
	return ret;
}

std::string		Reply::cannotSendToChannel(std::string const &nick, std::string const &channame)
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" 404 ");
	ret.append(nick);
	ret.append(" ");
	ret.append(channame);
	ret.append(" :Cannot send to nick/channel\r\n");

	return ret;
}

std::string		Reply::privmsgReply(Client *cli, std::string const &command, std::string const &tosend, std::string const &msg)
{
	std::string	ret;

	ret.append(":");
	ret.append(cli->nick_);
	ret.append("!");
	ret.append(cli->user_);
	ret.append("@");
	ret.append(cli->servername_);
	ret.append(" ");
	ret.append(command);
	ret.append(" ");
	ret.append(tosend);
	ret.append(" :");
	ret.append(msg);
	ret.append("\r\n");
	return ret;
}

std::string		Reply::failedPing(Client *cli, std::string const &command)
{
	std::string ret;
	
	ret.append(hostname);
	ret.append(" 461 ");
	ret.append(cli->nick_);
	ret.append(" ");
	ret.append(command);
	ret.append(" :Not enough parameters");
	ret.append("\r\n");
	return ret;
}

std::string		Reply::pongReply(std::string const &message)
{
	std::string ret;
	ret.append(hostname);
	ret.append(" PONG ");
	ret.append(hostname);
	ret.append(" ");
	ret.append(message);
	ret.append("\r\n");

	return ret;
}

std::string		Reply::luserReply(int users, int servers, std::string const &nick)
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" 251 ");
	ret.append(nick);
	ret.append(" :There are ");
	ret.append(std::to_string(users));
	ret.append(" users and 0 services on ");
	ret.append(std::to_string(servers));
	ret.append(" servers\r\n");

	return ret;
}

std::string		Reply::luseropReply(int operators, std::string const &nick)
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" 252 ");
	ret.append(nick);
	ret.append(" ");
	ret.append(std::to_string(operators));
	ret.append(" :operator(s) online\r\n");

	return ret;
}

std::string		Reply::luserunknownReply(int unknowns, std::string const &nick)
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" 253 ");
	ret.append(nick);
	ret.append(" ");
	ret.append(std::to_string(unknowns));
	ret.append(" :unknown connection(s)\r\n");

	return ret;
}

std::string		Reply::luserschannelsReply(int channels, std::string const &nick)
{
	//pendiente implementar
	std::string	ret;

	ret.append(hostname);
	ret.append(" 254 ");
	ret.append(nick);
	ret.append(" ");
	ret.append(std::to_string(channels));
	ret.append(" :channels formed\r\n");

	return ret;
}

std::string		Reply::lusersmeReply(int users, int servers, std::string const &nick)
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" 255 ");
	ret.append(nick);
	ret.append(" :I have ");
	ret.append(std::to_string(users));
	ret.append(" clients and ");
	ret.append(std::to_string(servers));
	ret.append(" servers\r\n");

	return ret;
}

std::string		Reply::whoisuserReply(Client *cli, std::string const & nick)
{
	std::string ret;
	
	ret.append(hostname);
	ret.append(" 311 ");
	ret.append(nick);
	ret.append(" ");
	ret.append(cli->nick_);
	ret.append(" ");
	ret.append(cli->user_);
	ret.append(" ");
	ret.append(cli->host_);
	ret.append(" * :");
	ret.append(cli->realname_);
	ret.append("\r\n");
	return ret;
}

std::string		Reply::whoischannelsReply(std::string const& nick, std::string const &mynick, std::string const &channelsname)
{
	std::string ret;
	
	ret.append(hostname);
	ret.append(" 319 ");
	ret.append(mynick);
	ret.append(" ");
	ret.append(nick);
	ret.append(" :");
	ret.append(channelsname);
	ret.append("\r\n");
	return ret;
}

std::string		Reply::whoisoperatorReply(std::string const& nick, std::string const &mynick)
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" 313 ");
	ret.append(mynick);
	ret.append(" ");
	ret.append(nick);
	ret.append(" :is an IRC operator\r\n");

	return ret;
}

std::string		Reply::whoisserverReply(std::string const &nick, std::string const &mynick, std::string const &newhost)
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" 312 ");
	ret.append(mynick);
	ret.append(" ");
	ret.append(nick);
	ret.append(" ");
	ret.append(newhost);
	ret.append(" :SP\r\n");

	return ret;
}

std::string		Reply::whoisidleReply(std::string const& nick, std::string const& mynick, std::time_t signontime)
{
	std::string	ret;
	time_t		end;
	int			total;

	time(&end);
	total = end - signontime;
	ret.append(hostname);
	ret.append(" 317 ");
	ret.append(mynick);
	ret.append(" ");
	ret.append(nick);
	ret.append(" ");
	ret.append(std::to_string(total));
	ret.append(" :seconds idle\r\n");

	return ret;
}

std::string		Reply::whoisendReply(std::string const &nick, std::string const &mynick)
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" 318 ");
	ret.append(mynick);
	ret.append(" ");
	ret.append(nick);
	ret.append(" :End of WHOIS list\r\n");

	return ret;
}

std::string		Reply::unknowncommandReply(std::string const &nick, std::string const &command)
{
	std::string ret;

	ret.append(hostname);
	ret.append(" 421 ");
	ret.append(nick);
	ret.append(" ");
	ret.append(command);
	ret.append(" :Unknown command\r\n");

	return ret;
}

std::string		Reply::nosuchchannelReply(std::string const &nick, std::string const &channel)
{
	std::string ret;

	ret.append(hostname);
	ret.append(" 403 ");
	ret.append(nick);
	ret.append(" ");
	ret.append(channel);
	ret.append(" :No such channel\r\n");

	return ret;
}

std::string		Reply::joinReply(Client *cli, std::string const &channame)
{
	std::string	ret;

	ret.append(":");
	ret.append(cli->nick_);
	ret.append("!~");
	ret.append(cli->user_);
	ret.append("@");
	ret.append(cli->host_);
	ret.append(" JOIN ");
	ret.append(channame);
	ret.append("\r\n");

	return ret;
}

std::string		Reply::inviteonlychanReply(std::string const &nick, std::string const &channame)
{
	std::string ret;

	ret.append(hostname);
	ret.append(" 473 ");
	ret.append(nick);
	ret.append(" ");
	ret.append(channame);
	ret.append(" :Cannot join channel (+i)\r\n");

	return ret;
}

std::string		Reply::topicReply(std::string const &nick, Channel *chan)
{
	std::string ret;

	ret.append(hostname);
	ret.append(" 332 ");
	ret.append(nick);
	ret.append(" ");
	ret.append(chan->name_);
	ret.append(" : ");
	ret.append(chan->topic_);
	ret.append("\r\n");

	return ret;
}

std::string		Reply::namesReply(std::string const &nick, Channel *chan)
{
	std::string ret;
	std::string	modes;
	std::vector<Client*>::iterator it;

	modes = chan->getModes();

	ret.append(hostname);
	ret.append(" 353 ");
	ret.append(nick);
	//Comprobamos si el canal es secreto o abierto
	if (modes.find('s') != std::string::npos)
		ret.append(" @ ");
	else
		ret.append(" = ");
	ret.append(chan->name_);
	ret.append(" :");
	for (it = chan->clients.begin(); it != chan->clients.end(); it++)
	{
		//Comprobamos si el cliente es invisible
		if (!(ft::checkMode("i", (*it)->mode_)))
		{
			if ((chan->getMemberModes((*it)->nick_)).find('o') != std::string::npos)
				ret.append("@");
			else if ((chan->getMemberModes((*it)->nick_)).find('v') != std::string::npos)
				ret.append("+");
			ret.append((*it)->nick_);
			ret.append(" ");
		}
	}
	ret.append("\r\n");

	return ret;
}

std::string		Reply::namesinchannelReply(std::string const &nick, Channel *chan)
{
	std::string ret;
	std::string	modes;
	std::vector<Client*>::iterator it;

	modes = chan->getModes();

	ret.append(hostname);
	ret.append(" 353 ");
	ret.append(nick);
	//Comprobamos si el canal es secreto o abierto
	if (modes.find('s') != std::string::npos)
		ret.append(" @ ");
	else
		ret.append(" = ");
	ret.append(chan->name_);
	ret.append(" :");
	for (it = chan->clients.begin(); it != chan->clients.end(); it++)
	{
		if ((chan->getMemberModes((*it)->nick_)).find('o') != std::string::npos)
			ret.append("@");
		else if ((chan->getMemberModes((*it)->nick_)).find('v') != std::string::npos)
			ret.append("+");
		ret.append((*it)->nick_);
		ret.append(" ");
	}
	ret.append("\r\n");

	return ret;
}

std::string		Reply::endnamesReply(std::string const &nick, std::string const &channame)
{
	std::string ret;

	ret.append(hostname);
	ret.append(" 366 ");
	ret.append(nick);
	ret.append(" ");
	ret.append(channame);
	ret.append(" :End of NAMES list\r\n");

	return ret;
}

std::string		Reply::notonchannelReply(std::string const &nick, std::string const &channame)
{
	std::string ret;

	ret.append(hostname);
	ret.append(" 442 ");
	ret.append(nick);
	ret.append(" ");
	ret.append(channame);
	ret.append(" :You're not on that channel\r\n");

	return ret;
}

std::string		Reply::notopicReply(std::string const &nick, std::string const &channame)
{
	std::string ret;

	ret.append(hostname);
	ret.append(" 331 ");
	ret.append(nick);
	ret.append(" ");
	ret.append(channame);
	ret.append(" :No topic is set\r\n");

	return ret;
}

std::string		Reply::topicReply(std::string const &nick, std::string const &channame, std::string const &topic)
{
	std::string ret;

	ret.append(hostname);
	ret.append(" 332 ");
	ret.append(nick);
	ret.append(" ");
	ret.append(channame);
	ret.append(" : ");
	ret.append(topic);
	ret.append("\r\n");

	return ret;
}

std::string		Reply::settopicReply(Client *cli, std::string const &channame, std::string const &topic)
{
	std::string	ret;

	ret.append(":");
	ret.append(cli->nick_);
	ret.append("!~");
	ret.append(cli->user_);
	ret.append("@");
	ret.append(cli->host_);
	ret.append(" TOPIC ");
	ret.append(channame);
	ret.append(" :");
	ret.append(topic);
	ret.append("\r\n");

	return ret;
}

std::string		Reply::partReply(Client *cli, std::string const &channame)
{
	std::string	ret;

	ret.append(":");
	ret.append(cli->nick_);
	ret.append("!~");
	ret.append(cli->user_);
	ret.append("@");
	ret.append(cli->host_);
	ret.append(" PART ");
	ret.append(channame);
	ret.append("\r\n");

	return ret;
}

std::string		Reply::youareoperReply(std::string const &nick)
{
	std::string ret;

	ret.append(hostname);
	ret.append(" 381 ");
	ret.append(nick);
	ret.append(" :You are now an IRC operator\r\n");

	return ret;
}

std::string		Reply::passwordmismatchReply(std::string const &nick)
{
	std::string ret;

	ret.append(hostname);
	ret.append(" 464 ");
	ret.append(nick);
	ret.append(" :Password incorrect\r\n");

	return ret;
}

std::string		Reply::modeunknownflagReply(std::string const &nick)
{
	std::string ret;

	ret.append(hostname);
	ret.append(" 501 ");
	ret.append(nick);
	ret.append(" :Unknown MODE flag\r\n");

	return ret;
}

std::string		Reply::userdontmatchReply(std::string const &nick)
{
	std::string ret;

	ret.append(hostname);
	ret.append(" 502 ");
	ret.append(nick);
	ret.append(" :Can't change mode for other users\r\n");

	return ret;
}

std::string		Reply::umodeReply(Client *cli)
{
	std::string ret;

	ret.append(hostname);
	ret.append(" 221 ");
	ret.append(cli->nick_);
	ret.append(" +");
	ret.append(cli->getModes());
	ret.append("\r\n");

	return ret;
}

std::string		Reply::modeReply(Client *cli, char operation, std::string modes)
{
	std::string	ret;

	ret.append(":");
	ret.append(cli->nick_);
	ret.append(" MODE ");
	ret.append(cli->nick_);
	ret.append(" ");
	ret.append(std::string(1, operation));
	ret.append(modes);
	ret.append("\r\n");

	return ret;
}

std::string		Reply::channelmodeisReply(std::string const &nick, Channel *chan)
{
	std::string ret;

	ret.append(hostname);
	ret.append(" 324 ");
	ret.append(nick);
	ret.append(" +");
	ret.append(chan->getModes());
	ret.append("\r\n");

	return ret;
}

std::string		Reply::chanoprivsneededReply(std::string const &nick, std::string const &channame)
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" 482 ");
	ret.append(nick);
	ret.append(" ");
	ret.append(channame);
	ret.append(" :You're not channel operator\r\n");

	return ret;
}

std::string		Reply::unknownmodeReply(std::string const &nick, std::string const &mode, std::string const &channame)
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" 472 ");
	ret.append(nick);
	ret.append(" ");
	ret.append(mode);
	ret.append(" :is unknown mode char to me for ");
	ret.append(channame);
	ret.append("\r\n");

	return ret;
}

std::string		Reply::channelmodeReply(Client *cli, std::string const &channame, char operation, std::string modes)
{
	std::string	ret;

	ret.append(":");
	ret.append(cli->nick_);
	ret.append("!~");
	ret.append(cli->user_);
	ret.append("@");
	ret.append(cli->host_);
	ret.append(" MODE ");
	ret.append(channame);
	ret.append(" ");
	ret.append(std::string(1, operation));
	ret.append(modes);
	ret.append("\r\n");

	return ret;
}

std::string		Reply::notuserinchannelReply(std::string const &nick, std::string const &tosend, std::string const &channame)
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" 441 ");
	ret.append(nick);
	ret.append(" ");
	ret.append(tosend);
	ret.append(" ");
	ret.append(channame);
	ret.append(" :They aren't on that channel\r\n");

	return ret;
}

std::string		Reply::userinchannelReply(std::string const &nick, std::string const &tosend, std::string const &channame)
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" 443 ");
	ret.append(nick);
	ret.append(" ");
	ret.append(tosend);
	ret.append(" ");
	ret.append(channame);
	ret.append(" :is already on channel\r\n");

	return ret;
}

std::string		Reply::usermemberchannelmodeReply(Client *cli, std::string const &channame, char operation, std::string const &tosend, std::string modes)
{
	std::string	ret;

	ret.append(":");
	ret.append(cli->nick_);
	ret.append("!~");
	ret.append(cli->user_);
	ret.append("@");
	ret.append(cli->host_);
	ret.append(" MODE ");
	ret.append(channame);
	ret.append(" ");
	ret.append(std::string(1, operation));
	ret.append(modes);
	ret.append(" ");
	ret.append(tosend);
	ret.append("\r\n");

	return ret;
}

std::string		Reply::awayReply(std::string const &nick, std::string const &replynick, std::string const &message)
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" 301 ");
	ret.append(nick);
	ret.append(" ");
	ret.append(replynick);
	ret.append(" :");
	ret.append(message);
	ret.append("\r\n");

	return ret;
}

std::string		Reply::unawayReply(std::string const &nick)
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" 305 ");
	ret.append(nick);
	ret.append(" :You are no longer marked as being away\r\n");

	return ret;
}

std::string		Reply::noawayReply(std::string const &nick)
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" 306 ");
	ret.append(nick);
	ret.append(" :You have been marked as being away\r\n");

	return ret;
}

std::string		Reply::listReply(std::string const &nick, Channel *chan, int number_clients)
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" 322 ");
	ret.append(nick);
	ret.append(" ");
	ret.append(chan->name_);
	ret.append(" ");
	ret.append(std::to_string(number_clients));
	ret.append(" :");
	ret.append(chan->topic_);
	ret.append("\r\n");

	return ret;
}

std::string		Reply::endlistReply(std::string const &nick)
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" 323 ");
	ret.append(nick);
	ret.append(" :End of LIST\r\n");

	return ret;
}

std::string		Reply::whoReply(std::string const &nick, Client *cli, std::string const &tosend, std::string const &channelmodes)
{
	std::string	ret;
	std::string modes;

	ret.append(hostname);
	ret.append(" 352 ");
	ret.append(nick);
	ret.append(" ");
	ret.append(tosend);
	ret.append(" ~");
	ret.append(cli->user_);
	ret.append(" ");
	ret.append(cli->host_);
	ret.append(" ");
	ret.append(hostname);
	ret.append(" ");
	ret.append(cli->nick_);
	ret.append(" ");
	modes = cli->getModes();
	if (modes.find('a') != std::string::npos)
		ret.append("G");
	else
		ret.append("H");
	if (modes.find('o') != std::string::npos)
		ret.append("*");
	if (channelmodes.find('o') != std::string::npos)
		ret.append("@");
	else if (channelmodes.find('v') != std::string::npos)
		ret.append("+");
	ret.append(" :0 ");
	ret.append(cli->realname_);
	ret.append("\r\n");
	return ret;
}

std::string		Reply::endwhoReply(std::string const &nick, std::string const &tosend)
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" 315 ");
	ret.append(nick);
	ret.append(" ");
	ret.append(tosend);
	ret.append(" :End of /WHO list\r\n");

	return ret;
}

std::string		Reply::invitingReply(std::string const &nick, std::string const &tosend, std::string const &channame)
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" 341 ");
	ret.append(nick);
	ret.append(" ");
	ret.append(tosend);
	ret.append(" ");
	ret.append(channame);
	ret.append("\r\n");

	return ret;
}

std::string		Reply::broadcastinvitingReply(Client *cli, std::string const &tosend, std::string const &channame)
{
	std::string	ret;

	ret.append(":");
	ret.append(cli->nick_);
	ret.append("!~");
	ret.append(cli->user_);
	ret.append("@");
	ret.append(cli->host_);
	ret.append(" INVITE ");
	ret.append(tosend);
	ret.append(" :");
	ret.append(channame);
	ret.append("\r\n");

	return ret;
}

std::string		Reply::broadcastkickReply(Client *cli, std::string const &tosend, std::string const &channame, std::string const &kick_message)
{
	std::string	ret;

	ret.append(":");
	ret.append(cli->nick_);
	ret.append("!~");
	ret.append(cli->user_);
	ret.append("@");
	ret.append(cli->host_);
	ret.append(" KICK ");
	ret.append(channame);
	ret.append(" ");
	ret.append(tosend);
	ret.append(" :");
	ret.append(kick_message);
	ret.append("\r\n");

	return ret;
}

std::string		Reply::badpasswordReply()
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" ERROR: Bad password\r\n");
	return ret;
}

std::string		Reply::passReply()
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" PASS PeerSecret 0210 IRCserv|0.5.1\r\n");
	return ret;
}

std::string		Reply::serverReply(std::string const &info)
{
	std::string	ret;
	std::string	ret_host(hostname);

	ret.append(hostname);
	ret.append(" SERVER ");
	ret_host.erase(0, 1);
	ret.append(ret_host);
	ret.append(" 1 :");
	ret.append(info);
	ret.append("\r\n");
	return ret;
}

std::string		Reply::multipleserverReply(Client *cli)
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" SERVER ");
	ret.append(cli->servername_);
	ret.append(" ");
	ret.append(std::to_string(cli->hopcount_));
	ret.append(" 3 :");
	ret.append(cli->info_);
	ret.append("\r\n");
	return ret;
}

std::string		Reply::multipleserverReply(Client *cli, int hop)
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" SERVER ");
	ret.append(cli->servername_);
	ret.append(" ");
	ret.append(std::to_string(hop));
	ret.append(" :");
	ret.append(cli->info_);
	ret.append("\r\n");
	return ret;
}

std::string		Reply::errnoprivilegesReply()
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" :Permission Denied- You're not an IRC operator\r\n");
	return ret;
}

std::string		Reply::sendPass()
{
	std::string	ret;

	ret.append("PASS PeerSecret 0210 IRCserv|0.5.1\r\n");
	return ret;
}

std::string		Reply::sendPass(std::string passwd)
{
	std::string	ret;

	ret.append("PASS ");
	ret.append(passwd);
	ret.append(" 0210 IRCserv|0.5.1\r\n");
	return ret;
}

std::string		Reply::sendServer(std::string const &info)
{
	std::string	ret;
	std::string	ret_host(hostname);

	ret.append("SERVER ");
	ret_host.erase(0, 1);
	ret.append(ret_host);
	ret.append(" :");
	ret.append(info);
	ret.append("\r\n");
	return ret;
}

std::string		Reply::sendallnicksReply(Client *cli)
{
	std::string ret;

	ret.append(hostname);
	ret.append(" NICK ");
	ret.append(cli->nick_);
	ret.append(" 1 ");
	ret.append(cli->user_);
	ret.append(" ");
	ret.append(cli->servername_);
	ret.append(" 1 +");
	ret.append(cli->getModes());
	ret.append(" :");
	ret.append(cli->realname_);
	ret.append("\r\n");
	return ret;
}

std::string		Reply::nickcollisionReply(std::string const &nick)
{
	std::string ret;

	ret.append(hostname);
	ret.append(" 436 ");
	ret.append(nick);
	ret.append(" :Nickname collision KILL\r\n");
	return ret;
}

std::string		Reply::changeNicknameForward(std::string const &oldnick, std::string const &newnick)
{
	std::string ret;

	ret.append(":");
	ret.append(oldnick);
	ret.append(" NICK ");
	ret.append(newnick);
	ret.append("\r\n");
	return ret;
}

std::string		Reply::privmsgForward(std::string const &nick, std::string const &tosend, std::string const &msg)
{
	std::string ret;

	ret.append(":");
	ret.append(nick);
	ret.append(" PRIVMSG ");
	ret.append(tosend);
	ret.append(" :");
	ret.append(msg);
	ret.append("\r\n");

	return ret;

}

std::string		Reply::joinForward(std::string const &nick, std::string const &channelname, std::string modes)
{
	std::string ret;

	ret.append(":");
	ret.append(nick);
	ret.append(" JOIN ");
	ret.append(channelname);
	if (modes.size() > 0)
	{
		ret.append("\a");
		ret.append(modes);
	}
	ret.append("\r\n");

	return ret;
}

std::string		Reply::joinForward(std::string const &nick, std::string const &channelname)
{
	std::string ret;

	ret.append(":");
	ret.append(nick);
	ret.append(" JOIN ");
	ret.append(channelname);
	ret.append("\r\n");

	return ret;
}

std::string		Reply::quitForward(std::string const &nick, std::string const &msg)
{
	std::string	ret;

	ret.append(":");
	ret.append(nick);
	ret.append(" QUIT :");
	ret.append(msg);
	ret.append("\r\n");

	return ret;
}

std::string		Reply::squitForward(std::string const &toerase, std::string const &info)
{
	std::string	ret;

	ret.append(hostname);
	ret.append(" SQUIT ");
	ret.append(toerase);
	ret.append(" :");
	ret.append(info);
	ret.append("\r\n");

	return ret;
}

std::string		Reply::partForward(std::string const &nick, std::string const &channame, std::string const &info)
{
	std::string	ret;

	ret.append(":");
	ret.append(nick);
	ret.append(" PART ");
	ret.append(channame);
	ret.append(" :");
	ret.append(info);
	ret.append("\r\n");

	return	ret;
}

std::string		Reply::timeReply(std::string const &nick, std::string const &message)
{
	std::string ret;
	std::string	host(hostname);

	ret.append(hostname);
	ret.append(" 391 ");
	ret.append(nick);
	ret.append(" ");
	host.erase(0, 1);
	ret.append(host);
	ret.append(" ");
	ret.append(message);

	return ret;

}

std::string		Reply::versionReply(std::string const &nick)
{
	std::string ret;
	std::string	host(hostname);

	ret.append(hostname);
	ret.append(" 351 ");
	ret.append(nick);
	ret.append(" cIRCunvolution-0.5.1. ");
	host.erase(0, 1);
	ret.append(host);
	ret.append(" :IPv4+IPv6+SSL+darwin+Linux\r\n");

	return ret;
}

std::string		Reply::supportedReply(std::string const &nick)
{
	std::string ret;

	ret.append(hostname);
	ret.append(" 005 ");
	ret.append(nick);
	ret.append(" CHARSET=UTF-8 CHANTYPES=# CHANMODES=mtsni MESSAGELEN=2048 :are supported in this server\r\n");

	return ret;
}

std::string		Reply::statslReply(Client *cli)
{
	std::string ret;
	time_t		end;
	int			total;

	time(&end);
	total = end - cli->signontime;

	ret.append(hostname);
	ret.append(" 211 ");
	ret.append(cli->nick_);
	ret.append(" ");
	ret.append(cli->nick_);
	ret.append("!");
	ret.append(cli->user_);
	ret.append("@");
	ret.append(cli->servername_);
	ret.append(" 0 ");
	ret.append(std::to_string(cli->msgSendnum));
	ret.append(" ");
	ret.append(std::to_string(cli->bytesSendnum));
	ret.append(" ");
	ret.append(std::to_string(cli->msgRecnum));
	ret.append(" ");
	ret.append(std::to_string(cli->bytesRecnum));
	ret.append(" :");
	ret.append(std::to_string(total));
	ret.append("\r\n");

	return ret;
}

std::string		Reply::statsendReply(std::string const &nick, std::string const &letter)
{
	std::string ret;

	ret.append(hostname);
	ret.append(" 219 ");
	ret.append(nick);
	ret.append(" ");
	ret.append(letter);
	ret.append(" :End of STATS report\r\n");

	return ret;
}

std::string		Reply::statsmReply(std::string const &nick, std::string const &command, int numcommands)
{
	std::string	ret;
	std::string def(command);

	std::transform(def.begin(), def.end(), def.begin(), ::toupper);
	ret.append(hostname);
	ret.append(" 212 ");
	ret.append(nick);
	ret.append(" ");
	ret.append(def);
	ret.append(" ");
	ret.append(std::to_string(numcommands));
	ret.append("\r\n");

	return ret;
}

std::string		Reply::statsuReply(std::string const &nick, time_t signontime)
{
	std::string ret;
	time_t		end;
	int			total;

	time(&end);
	total = end - signontime;

	ret.append(hostname);
	ret.append(" 242 ");
	ret.append(nick);
	ret.append(" :Server Up ");
	ret.append(std::to_string(total/(24 * 3600)));
	total = total % (24 * 3600);
	ret.append(" days ");
	ret.append(std::to_string(total/(3600)));
	total %= 3600;
	ret.append(":");
	ret.append(std::to_string(total/60));
	total %= 60;
	ret.append(":");
	ret.append(std::to_string(total));
	ret.append("\r\n");

	return ret;
}

std::string		Reply::traceReply(std::string const &nick, Client *cli)
{
	std::string	ret;

	ret.append(cli->servername_);
	ret.append(" 205 ");
	ret.append(nick);
	ret.append(" User ");
	ret.append(cli->rol_);
	ret.append(" ");
	ret.append(cli->nick_);
	ret.append("[");
	ret.append(cli->user_);
	ret.append("@");
	ret.append(cli->host_);
	ret.append("]\r\n");

	return ret;
}

std::string		Reply::traceendReply(std::string const &nick, Client *cli)
{
	std::string	ret;

	ret.append(cli->servername_);
	ret.append(" 262 ");
	ret.append(nick);
	ret.append(" ");
	ret.append(cli->nick_);
	ret.append(" :End of TRACE\r\n");

	return ret;
}

std::string		Reply::linksReply(std::string const &nick, Client *server)
{
	std::string ret;
	std::string	host(hostname);
	ret.append(hostname);
	ret.append(" 364 ");
	ret.append(nick);
	ret.append(" ");
	ret.append(server->servername_);
	ret.append(" ");
	host.erase(0, 1);
	ret.append(host);
	ret.append(" :");
	ret.append(std::to_string(server->hopcount_));
	ret.append(" ");
	ret.append(server->info_);
	ret.append("\r\n");
	return ret;
}
std::string		Reply::linksownservReply(std::string const &nick)
{
	std::string ret;
	std::string	host(hostname);
	ret.append(hostname);
	ret.append(" 364 ");
	ret.append(nick);
	ret.append(" ");
	host.erase(0, 1);
	ret.append(host);
	ret.append(" ");
	ret.append(hostname);
	ret.append(" :0 The best IRCserv in the world!\r\n");
	return ret;
}
std::string		Reply::endoflinksReply(std::string const &nick, std::string const &query)
{
	std::string ret;
	std::string	host(hostname);
	ret.append(hostname);
	ret.append(" 365 ");
	ret.append(nick);
	ret.append(" ");
	ret.append(query);
	ret.append(" :End of LINKS list\r\n");
	return ret;
}
std::string		Reply::infoReply(std::string const &nick, std::time_t t)
{
	std::string ret;
	std::string the_time;
	the_time = ctime(&t);
	the_time.erase(the_time.size()-1, 1);
	ret.append(hostname);
	ret.append(" 371 ");
	ret.append(nick);
	ret.append(" :circunvolution IRC server version 1.0 developed by @dalba-de and @jluknar-\r\n");
	ret.append(hostname);
	ret.append(" 371 ");
	ret.append(nick);
	ret.append(" :Birth Date: Mar 12 2021 at 21:34\r\n");
	ret.append(hostname);
	ret.append(" 371 ");
	ret.append(nick);
	ret.append(" :On-line since ");
	ret.append(the_time);
	ret.append(" (CEST)\r\n");
	return ret;
}
std::string		Reply::endofInfoReply(std::string const &nick)
{
	std::string ret;
	ret.append(hostname);
	ret.append(" 374 ");
	ret.append(nick);
	ret.append(" :End of INFO list\r\n");
	return ret;
}

std::string		Reply::adminReply(std::string const &nick)
{
	std::string ret;
	ret.append(hostname);
	ret.append(" 256 ");
	ret.append(nick);
	ret.append(" :Administrative info about circunvolution.42madrid.net\r\n");
	ret.append(hostname);
	ret.append(" 257 ");
	ret.append(nick);
	ret.append(" :You are using the circunvolution IRC server! For assistance, use 'stats'\r\n");
	ret.append(hostname);
	ret.append(" 258 ");
	ret.append(nick);
	ret.append(" :For further assistance, read the RFC 2812 standard, contact an admin or email\r\n");
	ret.append(hostname);
	ret.append(" 259 ");
	ret.append(nick);
	ret.append(" :dalba-de@student.42madrid.com / jluknar-@student.42madrid.com\r\n");

	return ret;
}

std::string		Reply::njoinReply(Channel *chan)
{
	std::string 					ret;
	std::vector<Client*>::iterator	it;
	std::string						modes;

	ret.append(hostname);
	ret.append(" NJOIN ");
	ret.append(chan->name_);
	ret.append(" :");
	for (it = chan->clients.begin(); it != chan->clients.end(); it++)
	{
		modes = chan->getMemberModes((*it)->nick_);
		if (modes.find("o") != std::string::npos)
		{
			ret.append("@");
			ret.append((*it)->nick_);
		}
		else
			ret.append((*it)->nick_);
		ret.append(",");
	}
	ret.erase((ret.length() - 1), 1);
	ret.append("\r\n");

	return ret;
}

/* ************************************************************************** */