#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include <cstring>
# include <unistd.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <netdb.h>
# include <arpa/inet.h>
# include <fcntl.h>
# include <errno.h>
# include <vector>
# include <map>
# include <ctime>
# include <iomanip>
# include <algorithm>
# include <openssl/ssl.h>
# include <openssl/err.h>
# include "Tools.hpp"
# include "Reply.hpp"
# define CMD_COUNT 33

class Client;
class Reply;
class Channel;

class Server
{

	public:
	
		Server(int argc, char** argv);
		~Server();

		void		init_stats();
		int			init_server();
		int			init_ssl();
		int			connect_to_serv();
		void		selectLoop();
		void		handleNewConnection();
		void		handleNewsslConnection();
		void		handleUserData(int socketfd);
		void		handleUserDatassl(int socketfd, int nbytes, Client *aux);
		int			sendAll(SSL *ssl, int socketfd, const char *buf, int len);
		Client*		getClient(int socketfd);
		Client*		getClientByNick(std::string const& nick);
		bool		findUserInServer(std::string const &nick);
		void		deleteClient(int socketfd);
		void		deleteClient(std::string nick);
		void		deleteServer(int socketfd);
		void		deleteServer(std::string servername);
		void		deleteClientFromChannel(int socketfd, std::vector<Client*> &chan_clients);
		void		deleteClientFromChannel(std::string const &nick, std::vector<Client*> &chan_clients);
		int     	processUserMessage(const char* message, Client *cli);
		void		welcome(Client *cli);
		void		broadcastRest(std::vector<Client*> chanclients, int clientsfd, std::string const &message);
		void		broadcastForward(std::string const &message);
		void		channelBroadcasting(Client *cli, std::string const &message);
		bool		findUserInChannel(std::string const &nick, Channel *chan);
		Channel*	channelExist(std::string const &chan_name);
		bool		isOperator(std::string modes);
		//Devuelve el socket del servidor donde esta el cliente
		int			getServerSocket(Client *cli);
		//SSL
		SSL_CTX		*create_context();
		void 		configure_context(SSL_CTX *ctx);

		int     	handle_nick(std::string nick, Client *cli);
		int			checkNicks(std::string const & nick);
		int     	handle_user(std::string user, Client *cli);
		int			handle_motd(std::string str, Client *cli);
		int			handle_quit(std::string message, Client *cli);
		int			handle_privmsg(std::string message, Client *cli);
		int			handle_notice(std::string message, Client *cli);
		int			handle_ping(std::string message, Client *cli);
		int			handle_pong(std::string message, Client *cli);
		int			handle_lusers(std::string message, Client *cli);
		int			handle_whois(std::string message, Client *cli);
		int			handle_join(std::string message, Client *cli);
		int			handle_part(std::string message, Client *cli);
		int			handle_topic(std::string message, Client *cli);
		int			handle_oper(std::string message, Client *cli);
		int			handle_mode(std::string message, Client *cli);
		int			handle_names(std::string message, Client *cli);
		int			handle_away(std::string message, Client *cli);
		int			handle_list(std::string message, Client *cli);
		int			handle_who(std::string message, Client *cli);
		int			handle_invite(std::string message, Client *cli);
		int			handle_kick(std::string message, Client *cli);
		int			handle_pass(std::string message, Client *cli);
		int			handle_server(std::string message, Client *cli);
		int			handle_connect(std::string message, Client *cli);
		int			handle_squit(std::string message, Client *cli);
		int			handle_time(std::string message, Client *cli);
		int			handle_version(std::string message, Client *cli);
		int			handle_stats(std::string message, Client *cli);
		int			handle_trace(std::string message, Client *cli);
		int			handle_links(std::string message, Client *cli);
		int			handle_info(std::string message, Client *cli);
		int			handle_admin(std::string message, Client *cli);
		int			handle_njoin(std::string message, Client *cli);

		int			forward_nick(std::string message, Client *cli);
		int			forward_privmsg(std::string message, Client *cli);
		int			forward_join(std::string message, Client *cli);
		int			forward_quit(std::string message, Client *cli);
		int			forward_part(std::string message, Client *cli);

	private:
		Server();
		Server &		operator=( Server const & rhs );
		Server( Server const & src );

		std::string							port;
		std::string							ssl_port;
		std::string							passw;
		std::string							host;
		std::string							port_network;
		std::string							passw_network;
		std::string							version;
		std::string							servername;
		std::string							usermodes;
		std::string							server_info;
		int									sfd;
		int									ssl_sfd;
		fd_set								master;
		fd_set								readfds;
		int									fdmax;
		std::time_t 						t;

		char 								s_addr[16];

		SSL_CTX* 							ctx;
		SSL									*ssl;

		std::vector<Client*>				clients;
		std::map<std::string, Channel*>		channels;
		std::vector<Client*>				servers;
		std::string 						commands[33] = {"NICK", "USER", "MOTD", "QUIT", "PRIVMSG", "NOTICE", "PING", 
											"PONG", "LUSERS", "WHOIS", "JOIN", "PART", "TOPIC", "OPER", "MODE", "NAMES",
											"AWAY", "LIST", "WHO", "INVITE", "KICK", "PASS", "SERVER", "CONNECT", "TIME",
											"VERSION", "SQUIT", "STATS", "TRACE", "LINKS", "INFO", "ADMIN", "NJOIN"};
		Reply								replies;
		std::map<std::string, int>			server_stats;

};

# include "Client.hpp"
# include "Channel.hpp"

#endif /* ********************************************************** SERVER_H */