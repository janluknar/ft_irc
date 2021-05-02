#ifndef REPLY_HPP
# define REPLY_HPP

# include <iostream>
# include <string>
# include <ctime>
# include <fstream>

class Client;
class Channel;

class Reply
{

	public:
		Reply();
		~Reply();

		void			init(std::string const &hostname_);

		std::string		noNickname();
		std::string		erroneusNickname(std::string const & nick);
		std::string		nickAlreadyInUse(std::string const & nick);
		std::string		needMoreParams(std::string const & command);
		std::string		alreadyRegistered();
		std::string		notRegistered();
		std::string		changeNickname(std::string const &oldnick, std::string const &username,
						std::string const &host, std::string const &newnick);
		std::string		welcomeReply(std::string const & nick, std::string const &version,
						std::time_t t, std::string const &usermodes, Client *cli);
		std::string		motdStart(std::string const & nick);
		std::string		motd(std::string const & nick, std::string const &line);
		std::string		motdEnd(std::string const & nick);
		std::string		clientQuit(std::string const &nick, std::string const &username, 
						std::string const &host,std::string const &msg);
		std::string		clientQuitFromChannel(std::string const &nick, std::string const &username,
						std::string const &host, std::string const &msg);
		std::string		noRecipient(std::string const& command, std::string const &nick);
		std::string		noTextToSend(std::string const &nick);
		std::string		noSuchNick(std::string const &nick, std::string const &no_nick);
		std::string		noSuchServer(std::string const& nick, std::string const& no_nick);
		std::string		cannotSendToChannel(std::string const &nick, std::string const &channame);
		std::string		privmsgReply(Client *cli, std::string const &command, std::string const &tosend, std::string const &msg);
		std::string		failedPing(Client *cli, std::string const &command);
		std::string		pongReply(std::string const &message);
		std::string		luserReply(int users, int servers, std::string const &nick);
		std::string		luseropReply(int operators, std::string const &nick);
		std::string		luserunknownReply(int unknowns, std::string const &nick);
		std::string		luserschannelsReply(int channels,  std::string const &nick);
		std::string		lusersmeReply(int users, int servers, std::string const &nick);
		std::string		whoisuserReply(Client *cli, std::string const &nick);
		std::string		whoisserverReply(std::string const& nick, std::string const &mynick, std::string const &newhost);
		std::string		whoisoperatorReply(std::string const& nick, std::string const &mynick);
		std::string		whoischannelsReply(std::string const& nick, std::string const &mynick, std::string const &channelsname);
		std::string		whoisidleReply(std::string const& nick, std::string const& mynick, std::time_t signontime);
		std::string		whoisendReply(std::string const& nick, std::string const &mynick);
		std::string		unknowncommandReply(std::string const &nick, std::string const &command);
		std::string		nosuchchannelReply(std::string const &nick, std::string const &channel);
		std::string		joinReply(Client *cli, std::string const &channame);
		std::string		inviteonlychanReply(std::string const &nick, std::string const &channame);
		std::string		topicReply(std::string const &nick, Channel *chan);
		std::string		namesReply(std::string const &nick, Channel *chan);
		std::string		namesinchannelReply(std::string const &nick, Channel *chan);
		std::string		endnamesReply(std::string const &nick, std::string const &channame);
		std::string		notonchannelReply(std::string const &nick, std::string const &channame);
		std::string		notopicReply(std::string const &nick, std::string const &channame);
		std::string		topicReply(std::string const &nick, std::string const &channame, std::string const &topic);
		std::string		settopicReply(Client *cli, std::string const &channame, std::string const &topic);
		std::string		partReply(Client *cli, std::string const &channame);
		std::string		youareoperReply(std::string const &nick);
		std::string		passwordmismatchReply(std::string const &nick);
		std::string		modeunknownflagReply(std::string const &nick);
		std::string		userdontmatchReply(std::string const &nick);
		std::string		umodeReply(Client *cli);
		std::string		modeReply(Client *cli, char operation, std::string modes);
		std::string		channelmodeisReply(std::string const &nick, Channel *chan);
		std::string		chanoprivsneededReply(std::string const &nick, std::string const &channame);
		std::string		unknownmodeReply(std::string const &nick, std::string const &mode, std::string const &channame);
		std::string		channelmodeReply(Client *cli, std::string const &channame, char operation, std::string modes);
		std::string		notuserinchannelReply(std::string const &nick, std::string const &tosend, std::string const &channame);
		std::string		userinchannelReply(std::string const &nick, std::string const &tosend, std::string const &channame);
		std::string		usermemberchannelmodeReply(Client *cli, std::string const &channame, char operation, std::string const &tosend, std::string modes);
		std::string		awayReply(std::string const &nick, std::string const &replynick, std::string const &message);
		std::string		unawayReply(std::string const &nick);
		std::string		noawayReply(std::string const &nick);
		std::string		listReply(std::string const &nick, Channel *chan, int number_clients);
		std::string		endlistReply(std::string const &nick);
		std::string		whoReply(std::string const &nick, Client *cli, std::string const &tosend, std::string const &channelmodes);
		std::string		endwhoReply(std::string const &nick, std::string const &tosend);
		std::string		invitingReply(std::string const &nick, std::string const &tosend, std::string const &channame);
		std::string		broadcastinvitingReply(Client *cli, std::string const &tosend, std::string const &channame);
		std::string		broadcastkickReply(Client *cli, std::string const &tosend, std::string const &channame, std::string const &kick_message);
		std::string		badpasswordReply();
		std::string		passReply();
		std::string		serverReply(std::string const &info);
		std::string		multipleserverReply(Client *cli);
		std::string		multipleserverReply(Client *cli, int hop);
		std::string		errnoprivilegesReply();
		std::string		sendPass();
		std::string		sendPass(std::string passwd);
		std::string		sendServer(std::string const &info);
		std::string		sendallnicksReply(Client *cli);
		std::string		nickcollisionReply(std::string const &nick);
		std::string		changeNicknameForward(std::string const &oldnick, std::string const &newnick);
		std::string		privmsgForward(std::string const &nick, std::string const &tosend, std::string const &msg);
		std::string		joinForward(std::string const &nick, std::string const &channelname, std::string modes);
		std::string		joinForward(std::string const &nick, std::string const &channelname);
		std::string		quitForward(std::string const &nick, std::string const &msg);
		std::string		squitForward(std::string const &toerase, std::string const &info);
		std::string		partForward(std::string const &nick, std::string const &channame, std::string const &info);

		std::string		timeReply(std::string const &nick, std::string const &message);
		std::string		versionReply(std::string const &nick);
		std::string		supportedReply(std::string const &nick);
		std::string		statslReply(Client *cli);
		std::string		statsendReply(std::string const &nick, std::string const &letter);
		std::string		statsmReply(std::string const &nick, std::string const &command, int numcommands);
		std::string		statsuReply(std::string const &nick, time_t signontime);
		std::string		traceReply(std::string const &nick, Client *cli);
		std::string		traceendReply(std::string const &nick, Client *cli);
		std::string		linksReply(std::string const &nick, Client *server);
		std::string		linksownservReply(std::string const &nick);
		std::string		endoflinksReply(std::string const &nick, std::string const &query);
		std::string		infoReply(std::string const &nick, std::time_t t);
		std::string		endofInfoReply(std::string const &nick);
		std::string		adminReply(std::string const &nick);

		std::string		njoinReply(Channel *chan);


	private:
		Reply( Reply const & src );
		Reply &		operator=( Reply const & rhs );

		std::string		hostname;

};

# include "Client.hpp"
# include "Channel.hpp"

#endif /* *********************************************************** REPLY_H */