#include "Server.hpp"

/*
** ------------------------------- CONSTRUCTOR --------------------------------
*/

Server::Server(int argc, char** argv)
{
	int aux;
	if (argc < 3 || argc > 4)
	{
		std::cerr << "Usage: ./ircserv [host:port_network:pass_network] <port> <password>" << std::endl;
		exit(1);
	}
	if (argc == 3)
	{
		port.append(argv[1]);
		aux = atoi(port.c_str());
		aux++;
		ssl_port = std::to_string(aux);
		passw.append(argv[2]);	
	}
	if (argc == 4)
	{
		port.append(argv[2]);
		aux = atoi(port.c_str());
		aux++;
		ssl_port = std::to_string(aux);
		passw.append(argv[3]);
		std::string s(argv[1]);
		host = s.substr(0, s.find(":"));
		s.erase(0, s.find(":") + 1);
		port_network = s.substr(0, s.find(":"));
		s.erase(0, s.find(":") + 1);
		passw_network.append(s);
	}
	version = "0.5.1";
	server_info = "The best IRCserv in the world!";
	usermodes = "aori";
	fdmax = 0;
	t = std::time(0);
	init_stats();
}

/*
** -------------------------------- DESTRUCTOR --------------------------------
*/

Server::~Server()
{
}


/*
** --------------------------------- OVERLOAD ---------------------------------
*/

/*
** --------------------------------- METHODS ----------------------------------
*/

void	Server::init_stats()
{
	server_stats["nick"] = 0;
	server_stats["user"] = 0;
	server_stats["motd"] = 0;
	server_stats["quit"] = 0;
	server_stats["privmsg"] = 0;
	server_stats["notice"] = 0;
	server_stats["lusers"] = 0;
	server_stats["whois"] = 0;
	server_stats["join"] = 0;
	server_stats["part"] = 0;
	server_stats["topic"] = 0;
	server_stats["oper"] = 0;
	server_stats["mode"] = 0;
	server_stats["names"] = 0;
	server_stats["away"] = 0;
	server_stats["list"] = 0;
	server_stats["who"] = 0;
	server_stats["invite"] = 0;
	server_stats["kick"] = 0;
	server_stats["connect"] = 0;
	server_stats["time"] = 0;
	server_stats["version"] = 0;
	server_stats["stats"] = 0;
	server_stats["trace"] = 0;
}

int		Server::init_server()
{
	struct addrinfo		hints;
	struct addrinfo		*result, *rp;
	int					s;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((s = getaddrinfo(0, this->port.c_str(), &hints, &result)) != 0)
	{
		std::cerr << "Error: " << strerror(s) << std::endl;
		return -1;
	}
	/* getaddrinfo() returns a list of address structures. Try each address until
	we successfully bind. If socket or bind  fails, we (close the socket and)
	try the next address. */
	int reuse = 1;
	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1)
			continue ;
		if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) != 0) //this way file socket descriptors can be reused
		{
			std::cerr << "Error: setsockopt(SO_REUSEADDR) failed" << std::endl;
			return -1;
		}
		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
			break ;
		close(sfd);
	}

	servername = "IRCserv." + this->port;
	std::cout << "Servername: " << servername << std::endl;
	replies.init(servername);

	freeaddrinfo(result);
	if (rp == NULL)
	{
		std::cerr << "Error: Could not bind" << std::endl;
		return -1;
	}

	// start accept client connections
	if (listen(sfd, 10) != 0)
	{
		std::cerr << "Error: listen failed" << std::endl;
		return -1;
	}
	std::cout << "Server connected. Accepting connections on port " << this->port << std::endl;
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
	fcntl(STDOUT_FILENO, F_SETFL, O_NONBLOCK);
	FD_ZERO(&master);
	FD_ZERO(&readfds);
	return 0;
}

int			Server::init_ssl()
{
	struct addrinfo		hints;
	struct addrinfo		*result, *ssl_rp;
	int					s;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((s = getaddrinfo(0, this->ssl_port.c_str(), &hints, &result)) != 0)
	{
		std::cerr << "Error: " << strerror(s) << std::endl;
		return -1;
	}
	/* getaddrinfo() returns a list of address structures. Try each address until
	we successfully bind. If socket or bind  fails, we (close the socket and)
	try the next address. */
	int reuse = 1;
	for (ssl_rp = result; ssl_rp != NULL; ssl_rp = ssl_rp->ai_next)
	{
		ssl_sfd = socket(ssl_rp->ai_family, ssl_rp->ai_socktype, ssl_rp->ai_protocol);
		if (ssl_sfd == -1)
			continue ;
		if (setsockopt(ssl_sfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) != 0) //this way file socket descriptors can be reused
		{
			std::cerr << "Error: setsockopt(SO_REUSEADDR) failed" << std::endl;
			return -1;
		}
		if (bind(ssl_sfd, ssl_rp->ai_addr, ssl_rp->ai_addrlen) == 0)
			break ;
		close(ssl_sfd);
	}

	freeaddrinfo(result);
	if (ssl_rp == NULL)
	{
		std::cerr << "Error: Could not bind" << std::endl;
		return -1;
	}

	// start accept client connections
	if (listen(ssl_sfd, 10) != 0)
	{
		std::cerr << "Error: listen failed" << std::endl;
		return -1;
	}
	std::cout << "Secure connections will be done through port " << ssl_port << std::endl;
	fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);
	fcntl(STDOUT_FILENO, F_SETFL, O_NONBLOCK);
	return 0;
}

int			Server::connect_to_serv()
{
	std::string						aux;
	struct addrinfo					hints;
	struct addrinfo					*result, *rp;
	int								s;
	int								clientfd;
	std::vector<Client*>::iterator	it;

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((s = getaddrinfo(this->host.c_str(), this->port_network.c_str(), &hints, &result)) != 0)
	{
		std::cerr << "Error: " << strerror(s) << std::endl;
		return -1;
	}
	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		clientfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1)
			continue ;
		if (connect(clientfd, rp->ai_addr, rp->ai_addrlen) == -1)
		{
			close(sfd);
			continue ;
		}
		break ;
	}
	freeaddrinfo(result);
	if (rp == NULL)
	{
		std::cerr << "Error: failed to connect" << std::endl;
		return -1;
	}
	FD_SET(clientfd, &master);
	if (clientfd > fdmax)
		fdmax = clientfd;
	fcntl(clientfd, F_SETFL, O_NONBLOCK);
	Client	*newclient = new Client(clientfd);
	clients.push_back(newclient);
	aux = replies.sendPass(this->passw_network);
	sendAll(ssl, clientfd, aux.c_str(), aux.length());
	aux.clear();
	aux = replies.sendServer(this->server_info);
	sendAll(ssl, clientfd, aux.c_str(), aux.length());
	return 0;
}

void		Server::selectLoop()
{
	FD_SET(sfd, &master);
	FD_SET(ssl_sfd, &master);
	if (sfd > fdmax)
		fdmax = sfd;
	if (ssl_sfd > fdmax)
		fdmax = ssl_sfd;

	//main loop
	for (;;)
	{
		readfds = master;
		if (select(fdmax + 1, &readfds, 0, NULL, NULL) == -1) //selects which file descriptor is ready to read and write
		{
			std::cout << "Error: select() failed" << std::endl;
			exit(EXIT_FAILURE);
		}

		for (int i = 0; i <= fdmax; i++)
		{
			if (FD_ISSET(i, &readfds)) //if fd is not 0, it is initalized and can be operated with
			{
				if (i == sfd)
					handleNewConnection();
				else if (i == ssl_sfd)
					handleNewsslConnection();
				else
					handleUserData(i);
			}
		}
	}
}

SSL_CTX*		Server::create_context()
{
	const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = SSLv23_server_method();

    ctx = SSL_CTX_new(method);
    if (!ctx)
	{
		perror("Unable to create SSL context");
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
    }

    return ctx;
}

void			Server::configure_context(SSL_CTX *ctx)
{
	SSL_CTX_set_ecdh_auto(ctx, 1);

    /* Set the key and cert */
    if (SSL_CTX_use_certificate_file(ctx, "ssl/server.crt", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
	exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, "ssl/server.key", SSL_FILETYPE_PEM) <= 0 ) {
        ERR_print_errors_fp(stderr);
	exit(EXIT_FAILURE);
    }
}

void			Server::handleNewsslConnection()
{
	SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
	ctx = create_context();
	configure_context(ctx);

	socklen_t			addrlen;
	int					newfd;
	struct sockaddr_in	remoteaddr;
	char				host_addr[50];

	addrlen = sizeof(remoteaddr);
	newfd = accept(ssl_sfd, (struct sockaddr *)&remoteaddr, &addrlen);  //creates new connected socket. not in listening state
	if (newfd == -1)
	{
		std::cout << "Error: Accept() failed" << std::endl;
		return ;
	}
	else
	{
		ssl = SSL_new(ctx);
		SSL_set_fd(ssl,newfd);
		if (SSL_accept(ssl) <= 0)
		{
            ERR_print_errors_fp(stderr);
		}
		FD_SET(newfd, &master);
		if (newfd > fdmax)
			fdmax = newfd;
		fcntl(newfd, F_SETFL, O_NONBLOCK); //operate on this file descriptor socket with non-blocking flags
		std::cout << "ircserv: new connection from " << inet_ntoa(remoteaddr.sin_addr) << " on socket " << newfd << std::endl;

	}
	Client	*aux = new Client(newfd, ssl);
	aux->host_ = inet_ntoa(remoteaddr.sin_addr);
	struct hostent *client_host = gethostbyname(aux->host_.c_str());
	aux->is_secure = true;
	sprintf(host_addr, "%s", (*client_host).h_name);
	printf("este host: %s\n", host_addr);
	aux->host_ = std::string(host_addr);
	clients.push_back(aux);
	std::string	notice(":circunvolution.42madrid.net NOTICE * :*** Please introduce your NICK and USER\r\n");
		if (sendAll(ssl, newfd, notice.c_str(), notice.length()))
			std::cout << "Error: send() failed" << std::endl;
}

void		Server::handleNewConnection()
{
	socklen_t			addrlen;
	int					newfd;
	struct sockaddr_in	remoteaddr;
	char				host_addr[50];

	addrlen = sizeof(remoteaddr);
	newfd = accept(sfd, (struct sockaddr *)&remoteaddr, &addrlen);  //creates new connected socket. not in listening state
	if (newfd == -1)
	{
		std::cout << "Error: Accept() failed" << std::endl;
		return ;
	}
	else
	{
		FD_SET(newfd, &master);
		if (newfd > fdmax)
			fdmax = newfd;
		fcntl(newfd, F_SETFL, O_NONBLOCK); //operate on this file descriptor socket with non-blocking flags
		std::cout << "ircserv: new connection from " << inet_ntoa(remoteaddr.sin_addr) << " on socket " << newfd << std::endl;

	}
	Client	*aux = new Client(newfd);
	aux->host_ = inet_ntoa(remoteaddr.sin_addr);
	struct hostent *client_host = gethostbyname(aux->host_.c_str());
	sprintf(host_addr, "%s", (*client_host).h_name);
	printf("este host: %s\n", host_addr);
	aux->host_ = std::string(host_addr);
	clients.push_back(aux);
	std::string	notice(":circunvolution.42madrid.net NOTICE * :*** Please introduce your NICK and USER\r\n");
		if (sendAll(ssl, newfd, notice.c_str(), notice.length()))
			std::cout << "Error: send() failed" << std::endl;
}

void		Server::handleUserData(int socketfd)
{
	int 	nbytes;
	Client*	aux;
	std::vector<Client*>::iterator	cliit;
	std::string	message;

	nbytes = 0;
	aux = getClient(socketfd);
	if(aux && aux->is_secure)
		handleUserDatassl(socketfd, nbytes, aux);
	else
	{
		nbytes = recv(socketfd, &aux->buf[aux->readpos], 2048 - aux->readpos, 0);
		if (nbytes < 0)
		{
			std::cout << "Error: recv() failed" << std::endl;
			deleteClient(socketfd);
		}
		else if (nbytes == 0)
		{
			std::cout << "Socket " << socketfd << " close by client" << std::endl;
			if (aux->rol_ == "user")
			{
				message = replies.quitForward(aux->nick_, " ");
				deleteClient(socketfd);
				for (cliit = servers.begin(); cliit != servers.end(); cliit++)
				{
					if ((*cliit)->hopcount_ == 1 && (*cliit)->socketfd != socketfd)
						sendAll((*cliit)->ssl, (*cliit)->socketfd, message.c_str(), message.length());
				}
			}
			else if (aux->rol_ == "server")
			{
				message = replies.squitForward(aux->servername_, "Server out of control");
				deleteServer(socketfd);
				deleteClient(socketfd);
				for (cliit = servers.begin(); cliit != servers.end(); cliit++)
				{
					if ((*cliit)->hopcount_ == 1 && (*cliit)->socketfd != socketfd)
						sendAll((*cliit)->ssl, (*cliit)->socketfd, message.c_str(), message.length());
				}
			}
			
		}
		else
		{
			std::cout << "bytes received: " << nbytes << std::endl;
			aux->readpos += nbytes;
			if (std::string(aux->buf).find('\n') != std::string::npos || aux->readpos >= 2048)
			{
				for (int i = 0; i < aux->readpos; i++)
				{
					if (aux->buf[i] == '\r' && aux->buf[i + 1] ==  '\n')
					{
						std::string temp(aux->buf, i);
						processUserMessage(temp.c_str(), aux);
						std::string temp1(aux->buf);
						temp1.erase(0, (i + 2));
						bzero(aux->buf, 2048);
						for (size_t j = 0; j < temp1.length(); j++)
							aux->buf[j] = temp1[j];
						aux->readpos -= (i + 2);
						i = 0;
						aux->msgSendnum++;
					}
				}
				aux->bytesSendnum += nbytes;
				bzero(aux->buf, 2048);
				aux->readpos = 0;
			}
		}
	}
}

void		Server::handleUserDatassl(int socketfd, int nbytes, Client *aux)
{
	nbytes = SSL_read(aux->ssl, &aux->buf[aux->readpos], 2048 - aux->readpos);
	if (nbytes < 0)
	{
		std::cout << "Error: recv() failed" << std::endl;
		deleteClient(socketfd);
	}
	else if (nbytes == 0)
	{
		std::cout << "Socket " << socketfd << " close by client" << std::endl;
		deleteClient(socketfd);
	}
	else
	{
		std::cout << "bytes received: " << nbytes << std::endl;
		aux->readpos += nbytes;
		if (std::string(aux->buf).find('\n') != std::string::npos || aux->readpos >= 2048)
		{
			for (int i = 0; i < aux->readpos; i++)
			{
				if (aux->buf[i] == '\r' && aux->buf[i + 1] ==  '\n')
				{
					std::string temp(aux->buf, i);
					processUserMessage(temp.c_str(), aux);
					std::string temp1(aux->buf);
					temp1.erase(0, (i + 2));
					bzero(aux->buf, 2048);
					for (size_t j = 0; j < temp1.length(); j++)
						aux->buf[j] = temp1[j];
					aux->readpos -= (i + 2);
					i = 0;
					aux->msgSendnum++;
				}
			}
			aux->bytesSendnum += nbytes;
			bzero(aux->buf, 2048);
			aux->readpos = 0;
		}
	}
}

int       Server::processUserMessage(const char *message, Client *cli)
{
	std::string               aux(message);
	std::vector<std::string>  words;
	int (Server::*CmdHandler[33])(std::string, Client *)  = {&Server::handle_nick, &Server::handle_user, &Server::handle_motd, &Server::handle_quit,
															&Server::handle_privmsg, &Server::handle_notice, &Server::handle_ping, &Server::handle_pong,
															&Server::handle_lusers, &Server::handle_whois, &Server::handle_join, &Server::handle_part,
															&Server::handle_topic, &Server::handle_oper, &Server::handle_mode, &Server::handle_names,
															&Server::handle_away, &Server::handle_list, &Server::handle_who, &Server::handle_invite,
															&Server::handle_kick, &Server::handle_pass, &Server::handle_server, &Server::handle_connect,
															&Server::handle_time, &Server::handle_version, &Server::handle_squit, &Server::handle_stats,
															&Server::handle_trace, &Server::handle_links, &Server::handle_info, &Server::handle_admin,
															&Server::handle_njoin};

	ft::split(aux, words);
	std::transform(words[0].begin(), words[0].end(), words[0].begin(), ::toupper);
	for (int i = 0; i < CMD_COUNT; i++)
	{
		if (*(words.begin()) == commands[i])
		{
			(this->*CmdHandler[i])(aux, cli);
			return 0;
		}
	}
	if (words[0][0] == ':')
	{
		for (int i = 0; i < CMD_COUNT; i++)
		{
			if (words[1] == commands[i])
			{
				(this->*CmdHandler[i])(aux, cli);
				return 0;
			}
		}
	}
	aux.clear();
	aux = replies.unknowncommandReply(cli->nick_, words[0]);
	sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	return 0;
}

Client*		Server::getClient(int socketfd)
{
	std::vector<Client*>::iterator it = clients.begin();

	for ( ; it != clients.end(); it++)
	{
		if ((*it)->getSocket() == socketfd)
			return *it;
	}
	return nullptr;
}

Client*		Server::getClientByNick(std::string const &nick)
{
	std::vector<Client*>::iterator it = clients.begin();

	for ( ; it != clients.end(); it++)
	{
		if ((*it)->nick_ == nick)
			return *it;
	}
	return nullptr;
}

bool		Server::findUserInServer(std::string const &nick)
{
	std::vector<Client*>::iterator it = clients.begin();

	for ( ; it != clients.end(); it++)
	{
		if ((*it)->nick_ == nick)
			return true;
	}
	return false;
}

void		Server::deleteClient(int socketfd)
{
	std::vector<Client*>::iterator it = clients.begin();
	std::vector<Client*>::iterator ite = clients.end();

	close(socketfd);
	FD_CLR(socketfd, &master);
	for ( ; it != ite; it++)
	{
		if ((*it)->getSocket() == socketfd)
		{
			delete *it;
			clients.erase(it);
		}
			
	}
}

void		Server::deleteClient(std::string nick)
{
	std::vector<Client*>::iterator it = clients.begin();
	std::vector<Client*>::iterator ite = clients.end();

	for ( ; it != ite; it++)
	{
		if ((*it)->nick_ == nick)
		{
			delete *it;
			clients.erase(it);
		}
			
	}
}

void		Server::deleteServer(int socketfd)
{
	std::vector<Client*>::iterator it = servers.begin();
	std::vector<Client*>::iterator ite = servers.end();

	close(socketfd);
	FD_CLR(socketfd, &master);
	for ( ; it != ite; it++)
	{
		if ((*it)->socketfd == socketfd)
			servers.erase(it);
	}
}

void		Server::deleteServer(std::string servername)
{
	std::vector<Client*>::iterator it = servers.begin();
	std::vector<Client*>::iterator ite = servers.end();

	for ( ; it != ite; it++)
	{
		if ((*it)->servername_ == servername)
			servers.erase(it);
	}
}

void		Server::deleteClientFromChannel(int socketfd, std::vector<Client*> &chan_clients)
{
	std::vector<Client*>::iterator it = chan_clients.begin();
	std::vector<Client*>::iterator ite = chan_clients.end();

	for ( ; it != ite; it++)
	{
		if ((*it)->socketfd == socketfd)
			chan_clients.erase(it);
	}
}

void		Server::deleteClientFromChannel(std::string const &nick, std::vector<Client*> &chan_clients)
{
	std::vector<Client*>::iterator it = chan_clients.begin();
	std::vector<Client*>::iterator ite = chan_clients.end();

	for ( ; it != ite; it++)
	{
		if ((*it)->nick_ == nick)
			chan_clients.erase(it);
	}
}

int			Server::sendAll(SSL *ssl, int socketfd, const char *buf, int len)
{
	int total = 0;
	int	bytesleft = len;
	int n;
	Client	*newclient = getClient(socketfd);

	if(newclient->is_secure)
	{
		while (total < len)
		{
			n = SSL_write(ssl, buf + total, bytesleft);
			if (n == -1)
				break ;
			total  += n;
			bytesleft -= n;
		}
	}
	else
	{
		while (total < len)
		{
			n = send(socketfd, buf + total, bytesleft, 0);
			if (n == -1)
				break ;
			total  += n;
			bytesleft -= n;
		}
	}
	newclient->bytesRecnum += len;
	newclient->msgRecnum++;

	return n == -1 ? -1 : 0;
}

int		Server::checkNicks(std::string const & nick)
{
	std::vector<Client*>::iterator it = clients.begin();
	std::vector<Client*>::iterator ite = clients.end();

	for ( ; it != ite; it++)
	{
		if ((*it)->nick_ == nick)
			return 1;
	}
	return 0;
}

void	Server::welcome(Client *cli)
{
	std::string aux;

	aux = replies.welcomeReply(cli->nick_, version, t, usermodes, cli);
	sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	handle_lusers(aux, cli);
	handle_motd(aux, cli);
}

void	Server::broadcastRest(std::vector<Client*> chanclients, int clientsfd, std::string const &message)
{
	std::vector<Client*>::iterator it = chanclients.begin();

	for ( ; it != chanclients.end(); it++)
	{
		if ((*it)->socketfd != clientsfd && (*it)->servername_ == this->servername)
			sendAll((*it)->ssl, (*it)->socketfd, message.c_str(), message.length());
	}
}

void	Server::broadcastForward(std::string const &message)
{
	std::vector<Client*>::iterator	it = servers.begin();

	for ( ; it != servers.end(); it++)
	{
		if ((*it)->hopcount_ == 1)
			sendAll((*it)->ssl, (*it)->socketfd, message.c_str(), message.length());
	}
	
}

void	Server::channelBroadcasting(Client *cli, std::string const &message)
{
	std::map<std::string, Channel*>::iterator	it;

	it = channels.begin();
	for ( ; it != channels.end(); it++)
	{
		if (findUserInChannel(cli->nick_, it->second))
			broadcastRest(it->second->clients, cli->socketfd, message);
	}
}

bool	Server::findUserInChannel(std::string const &nick, Channel *chan)
{
	std::vector<Client*>::iterator it = chan->clients.begin();

	for ( ; it != chan->clients.end(); it++)
	{
		if ((*it)->nick_ == nick)
			return true;
	}
	return false;
}

Channel*	Server::channelExist(std::string const &chan_name)
{
	std::map<std::string, Channel*>::iterator	it;

	it = channels.begin();
	for ( ; it != channels.end(); it++)
	{
		if (it->first == chan_name)
			return it->second;
	}
	return nullptr;
}

bool		Server::isOperator(std::string modes)
{
	for (size_t i = 0; i < modes.length(); i++)
	{
		if (modes[i] == 'o')
			return true;
	}
	return false;
}

int			Server::getServerSocket(Client *cli)
{
	std::vector<Client*>::iterator it = servers.begin();

	for ( ; it != servers.end(); it++)
	{
		if ((*it)->servername_ == cli->servername_)
			return (*it)->socketfd;
	}
	return 0;
}

/*
// AQUI COMIENZAN LOS COMANDOS
*/

int     Server::handle_nick(std::string nick, Client *cli)
{
	std::vector<std::string>				parser;
	std::string								def_nick;
	std::string								aux;
	std::vector<Client *>::iterator			it;
	std::map<std::string, int>::iterator	statsit;

	statsit = server_stats.find("nick");
	statsit->second += 1;
	if (cli->rol_ == "server")
		return (forward_nick(nick, cli));

	ft::split(nick, parser);
	if (parser.size() == 1)
	{
		aux = replies.noNickname();
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	def_nick = parser[1];
	if (ft::checkString(def_nick))
	{
		aux = replies.erroneusNickname(def_nick);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	else if (checkNicks(def_nick))
	{
		aux = replies.nickAlreadyInUse(def_nick);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	else
	{
		if (!cli->nick_.empty() && ft::checkMode("r", cli->mode_))
		{
			for(it = servers.begin(); it != servers.end(); it++)
			{
				aux = replies.changeNicknameForward(cli->nick_, def_nick);
				sendAll(ssl, (*it)->socketfd, aux.c_str(), aux.length());
			}
			aux.clear();
			aux = replies.changeNickname(cli->nick_, cli->user_, cli->host_, def_nick);
			channelBroadcasting(cli, aux);
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
			cli->setNickname(def_nick);
			return 0;
		}
		cli->setNickname(def_nick);
		if (!cli->user_.empty())
		{
			cli->setMode("r", '+', cli->mode_);
			cli->setRol("user");
			welcome(cli);
			for(it = servers.begin(); it != servers.end(); it++)
			{
				aux = replies.sendallnicksReply(cli);
				sendAll(ssl, (*it)->socketfd, aux.c_str(), aux.length());
			}
		}	
	}
		
	return 0;
}

int     Server::handle_user(std::string user, Client *cli)
{
	std::vector<std::string>				parser;
	std::string								def_user;
	std::string								aux;
	std::vector<Client *>::iterator			it;
	std::map<std::string, int>::iterator	statsit;

	statsit = server_stats.find("user");
	statsit->second += 1;
	ft::split(user, parser);
	if (parser.size() < 5)
	{
		aux = replies.needMoreParams("USER");
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	if (ft::checkMode("r", cli->mode_))
	{
		aux = replies.alreadyRegistered();
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	cli->setUsername(parser[1]);
	if (parser[4][0] == ':')
		parser[4].erase(0, 1);
	for (size_t i = 4; i < parser.size(); i++)
	{
		def_user.append(parser[i]);
		def_user.append(" ");
	}
	cli->setRealname(def_user);
	if (!cli->nick_.empty())
	{
		cli->setMode("r", '+', cli->mode_);
		cli->setRol("user");
		cli->setServername(this->servername);
		welcome(cli);
		for(it = servers.begin(); it != servers.end(); it++)
		{
			if ((*it)->socketfd != 0)
			{
				aux = replies.sendallnicksReply(cli);
				sendAll(ssl, (*it)->socketfd, aux.c_str(), aux.length());
			}
		}
	}	
	return 0;
}

int		Server::handle_motd(std::string str, Client *cli)
{
	(void)str;
	std::string								aux;
	std::string								line;
	std::ifstream							myfile ("MOTD");
	std::map<std::string, int>::iterator	statsit;

	statsit = server_stats.find("motd");
	statsit->second += 1;

	if (!(ft::checkMode("r", cli->mode_)))
	{
		aux = replies.notRegistered();
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}

	aux = replies.motdStart(cli->nick_);
	sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());

	aux.clear();
	if (myfile.is_open())
	{
		while ( getline (myfile,line) )
		{
			aux.clear();
			aux = replies.motd(cli->nick_, line);
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		}
		myfile.close();
	}
	else 
		std::cout << "Unable to open file\n";

	aux.clear();
	aux = replies.motdEnd(cli->nick_);
	sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	return 0;
}

int		Server::handle_quit(std::string message, Client *cli )
{
	std::vector<std::string>					parser;
	std::string									quit_msg;
	std::string									aux;
	std::map<std::string, Channel*>::iterator	it;
	std::vector<Client*>::iterator				cliit;
	std::map<std::string, int>::iterator		statsit;

	statsit = server_stats.find("quit");
	statsit->second += 1;

	if (cli->rol_ == "server")
		return (forward_quit(message, cli));
	ft::split(message, parser, ':');
	if (parser.size() > 1)
		quit_msg = *(parser.end() - 1);
	else
		quit_msg = "Client Quit";
	aux = replies.clientQuitFromChannel(cli->nick_, cli->user_, cli->host_, quit_msg);
	it = channels.begin();
	for ( ; it != channels.end(); it++)
	{
		if (findUserInChannel(cli->nick_, it->second))
		{
			deleteClientFromChannel(cli->socketfd, it->second->clients);
			broadcastRest(it->second->clients, cli->socketfd, aux);
		}
	}
	aux.clear();
	aux = replies.clientQuit(cli->nick_, cli->user_, cli->host_, quit_msg);
	sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	aux.clear();
	aux = replies.quitForward(cli->nick_, quit_msg);
	for (cliit = servers.begin(); cliit != servers.end(); cliit++)
	{
		if ((*cliit)->hopcount_ == 1)
			sendAll((*cliit)->ssl, (*cliit)->socketfd, aux.c_str(), aux.length());
	}
	deleteClient(cli->socketfd);
	return 0;
}

int		Server::handle_privmsg(std::string message, Client *cli)
{
	std::vector<std::string>				parser;
	std::vector<std::string>				parser2;
	std::string								msg;
	std::string								aux;
	std::string								client;
	std::string								modes;
	Client									*newclient;
	Client									*newserver;
	Channel									*newchannel;
	std::vector<Client*>::iterator			it;
	std::map<std::string, int>::iterator	statsit;

	statsit = server_stats.find("privmsg");
	statsit->second += 1;

	if (cli->rol_ == "server")
		return (forward_privmsg(message, cli));

	ft::split(message, parser);
	if (parser.size() == 1)
	{
		aux = replies.noRecipient("PRIVMSG", cli->nick_);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}

	if (parser.size() >= 3 && parser[2][0] != ':')
		parser[2].insert(0, ":") ;
	message.clear();
	for (size_t i = 0; i < parser.size(); i++)
	{
		message.append(parser[i]);
		message.append(" ");
	}
	parser.clear();

	ft::split(message, parser, ':');
	if (parser.size() == 1 || parser[1].size() == 0)
	{
		aux = replies.noTextToSend(cli->nick_);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	ft::split(parser[0], parser2);
	if (parser2.size() == 1 || parser2[1].size() == 0)
	{
		aux = replies.noTextToSend(cli->nick_);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	else
	{
		//MENSAJES ENTRE CANALES
		if ((newchannel = channelExist(parser2[1])) != nullptr)
		{
			client = parser2[1];
			if (findUserInChannel(cli->nick_, newchannel))
			{
				if ((newchannel->getModes()).find('m') != std::string::npos)
				{
					modes = newchannel->clientsmode.find(cli->nick_)->second;
					if ((modes.find('o') == std::string::npos) && (modes.find('v') == std::string::npos))
					{
						aux = replies.cannotSendToChannel(cli->nick_, newchannel->name_);
						sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
						return 1;
					}
				}
			}
			else
			{
				if ((newchannel->getModes()).find('n') != std::string::npos)
				{
					aux = replies.cannotSendToChannel(cli->nick_, newchannel->name_);
					sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
					return 1;
				}
			}
			aux = replies.privmsgReply(cli, "PRIVMSG", client, *(parser.end() - 1));
			broadcastRest(newchannel->clients, cli->socketfd, aux);
			aux.clear();
			aux = replies.privmsgForward(cli->nick_, client, *(parser.end() - 1));
			broadcastForward(aux);
		}
		//MENSAJES ENTRE CLIENTES
		else
		{
			client = parser2[1];
			newclient = getClientByNick(client);
			if (newclient)
			{
				//COMPRUEBO SI ESTA EN MI MISMO SERVIDOR
				if (newclient->servername_ == this->servername)
				{
					aux = replies.privmsgReply(cli, "PRIVMSG", client, *(parser.end() - 1));
					sendAll(ssl, newclient->socketfd, aux.c_str(), aux.length());
					if ((newclient->getModes()).find('a') != std::string::npos)
					{
						aux.clear();
						aux = replies.awayReply(cli->nick_, newclient->nick_, newclient->away);
						sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
					}
				}
				else
				{
					int serverfd = getServerSocket(newclient);
					aux = replies.privmsgForward(cli->nick_, client, *(parser.end() - 1));
					if (!serverfd)
					{
						for (it = servers.begin(); it != servers.end(); it++)
						{
							if ((*it)->hopcount_ == 1)
								sendAll((*it)->ssl, (*it)->socketfd, aux.c_str(), aux.length());
						}
					}
					else
					{
						newserver = getClient(serverfd);
						sendAll(newserver->ssl, serverfd, aux.c_str(), aux.length());
					}
				}
			}
			else
			{
				aux = replies.noSuchNick(cli->nick_, client);
				sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
				return 1;
			}
		}
	}
	return 0;
}

int		Server::handle_notice(std::string message, Client *cli)
{
	std::vector<std::string>				parser;
	std::vector<std::string>				parser2;
	std::string								msg;
	std::string								aux;
	std::string								client;
	std::string								modes;
	Client									*newclient;
	Channel									*newchannel;
	std::map<std::string, int>::iterator	statsit;

	statsit = server_stats.find("notice");
	statsit->second += 1;

	ft::split(message, parser);
	if (parser.size() == 1)
		return 1;

	if (parser.size() >= 3 && parser[2][0] != ':')
		parser[2].insert(0, ":") ;
	message.clear();
	for (size_t i = 0; i < parser.size(); i++)
	{
		message.append(parser[i]);
		message.append(" ");
	}
	parser.clear();

	ft::split(message, parser, ':');
	if (parser.size() == 1 || parser[1].size() == 0)
		return 1;
	ft::split(parser[0], parser2);
	if (parser2.size() == 1 || parser2[1].size() == 0)
		return 1;
	else
	{
		if ((newchannel = channelExist(parser2[1])) != nullptr)
		{
			client = parser2[1];
			if (findUserInChannel(cli->nick_, newchannel))
			{
				if ((newchannel->getModes()).find('m') != std::string::npos)
				{
					modes = newchannel->clientsmode.find(cli->nick_)->second;
					if ((modes.find('o') == std::string::npos) && (modes.find('v') == std::string::npos))
						return 1;
				}
			}
			else
			{
				if ((newchannel->getModes()).find('n') != std::string::npos)
					return 1;
			}
			aux = replies.privmsgReply(cli, "NOTICE", client, *(parser.end() - 1));
			broadcastRest(newchannel->clients, cli->socketfd, aux);
		}
		else
		{
			client = parser2[1];
			newclient = getClientByNick(client);
			if (newclient)
			{
				aux = replies.privmsgReply(cli, "NOTICE", client, *(parser.end() - 1));
				sendAll(ssl, newclient->socketfd, aux.c_str(), aux.length());
			}
			else
				return 1;
		}
	}
	return 0;
}

int 	Server::handle_pong(std::string message, Client *cli)
{
	(void)message;
	(void)cli;
	return 0;
}


int		Server::handle_ping(std::string message, Client *cli) /*funciona igual con delimitador que sin delimitador*/
{
	std::vector<std::string>	parser;
	std::string					aux;

	if (!(ft::checkMode("r", cli->mode_)))
	{
		aux = replies.notRegistered();
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}

	ft::split(message, parser);
	if (parser.size() == 1 || parser[1].size() == 0)
	{
		aux = replies.failedPing(cli, "PING");
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	if (parser[1][0] != ':')
		parser[1].insert(0, ":");
	aux = replies.pongReply(parser[1]);
	sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	return 0;
}

int		Server::handle_lusers(std::string message, Client *cli)
{
	(void)message;
	std::string								aux;
	std::string								usermodes;
	int										users = 0;
	int										operators = 0;
	int										unknowns = 0;
	int										servers;
	std::map<std::string, int>::iterator	statsit;

	statsit = server_stats.find("lusers");
	statsit->second += 1;

	std::vector<Client *>::iterator it = clients.begin();

	if (!(ft::checkMode("r", cli->mode_)))
	{
		aux = replies.notRegistered();
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}

	for ( ; it != clients.end(); it++)
	{
		usermodes = (*it)->getModes();
		if ((*it)->rol_ == "user")
			users++;
		else if ((*it)->rol_ == "unknown")
			unknowns++;
		if (usermodes.find('o') != std::string::npos)
			operators++;
	}
	servers = this->servers.size();
	aux = replies.luserReply(users, servers, cli->nick_);
	sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	aux.clear();
	aux = replies.luseropReply(operators, cli->nick_);
	sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	aux.clear();
	aux = replies.luserunknownReply(unknowns, cli->nick_);
	sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	aux.clear();
	aux = replies.luserschannelsReply(channels.size(), cli->nick_);
	sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	aux.clear();
	aux = replies.lusersmeReply(users, servers, cli->nick_);
	sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	return 0;
}

int		Server::handle_whois(std::string message, Client *cli)
{
	std::vector<std::string>					parser;
	std::string									aux;
	Client										*newclient;
	std::map<std::string, Channel*>::iterator	mapit;
	std::string									channelsname;
	std::string									channelmodes;
	std::string									clientmodes;
	std::map<std::string, int>::iterator		statsit;

	statsit = server_stats.find("whois");
	statsit->second += 1;

	if (!(ft::checkMode("r", cli->mode_)))
	{
		aux = replies.notRegistered();
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}

	ft::split(message, parser);
	if (parser.size() == 1 || parser[1].size() == 0)
	{
		aux = replies.needMoreParams("WHOIS");
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	newclient = getClientByNick(parser[1]);
	if (newclient == nullptr)
	{
		aux = replies.noSuchNick(cli->nick_, parser[1]);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	else
	{
		aux = replies.whoisuserReply(newclient, cli->nick_);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		aux.clear();
		//WHOIS CHANNELS
		for (mapit = channels.begin(); mapit != channels.end(); mapit++)
		{
			if (findUserInChannel(newclient->nick_, mapit->second))
			{
				channelmodes = mapit->second->getMemberModes(newclient->nick_);
				if (channelmodes.find('o') != std::string::npos)
					channelsname.append("@");
				else if (channelmodes.find('v') != std::string::npos)
					channelsname.append("+");
				channelsname.append(mapit->second->name_);
				channelsname.append(" ");
			}
		}
		aux = replies.whoischannelsReply(newclient->nick_, cli->nick_, channelsname);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		//WHOIS SERVER
		aux.clear();
		aux = replies.whoisserverReply(newclient->nick_, cli->nick_, newclient->servername_);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		//WHOIS AWAY
		clientmodes = newclient->getModes();
		if (clientmodes.find('a') != std::string::npos)
		{
			aux.clear();
			aux = replies.awayReply(cli->nick_, newclient->nick_, newclient->away);
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		}
		//WHOIS OPERATOR
		if (clientmodes.find('o') != std::string::npos)
		{
			aux.clear();
			aux = replies.whoisoperatorReply(newclient->nick_, cli->nick_);
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		}
		//WHOIS IDLE
		aux.clear();
		aux = replies.whoisidleReply(newclient->nick_, cli->nick_, newclient->signontime);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		aux.clear();
		aux = replies.whoisendReply(newclient->nick_, cli->nick_);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	}
	return 0;
}

int		Server::handle_join(std::string message, Client *cli)
{
	std::vector<std::string>					parser;
	std::map<std::string, Channel*>::iterator	it;
	std::vector<Client *>::iterator				vectorit;
	std::string									aux;
	Channel										*newchannel;
	std::string									channelmodes;
	std::vector<std::string>::iterator			invitesit;
	bool										isInvited = false;
	std::map<std::string, int>::iterator		statsit;

	statsit = server_stats.find("join");
	statsit->second += 1;

	if (cli->rol_ == "server")
		return (forward_join(message, cli));

	if (!(ft::checkMode("r", cli->mode_)))
	{
		aux = replies.notRegistered();
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}

	ft::split(message, parser);
	if (parser.size() == 1 || parser[1].size() == 0)
	{
		aux = replies.needMoreParams("JOIN");
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	if (parser[1][0] != '#')
	{
		aux = replies.nosuchchannelReply(cli->nick_, parser[1]);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	std::transform(parser[1].begin(), parser[1].end(), parser[1].begin(), ::tolower);
	if ((it = channels.find(parser[1])) == channels.end())
	{
		newchannel = new Channel(cli, parser[1]);
		std::pair<std::string, Channel*> pair;
		pair = std::make_pair(parser[1], newchannel);
		channels.insert(pair);
		std::pair<std::string, std::string> strpair;
		strpair = std::make_pair(cli->nick_, "o");
		newchannel->clientsmode.insert(strpair);
		aux = replies.joinReply(cli, newchannel->name_);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		aux.clear();
		aux = replies.namesinchannelReply(cli->nick_, newchannel);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		aux.clear();
		aux = replies.endnamesReply(cli->nick_, newchannel->name_);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		aux.clear();
		aux = replies.joinForward(cli->nick_, parser[1], "o");
		for (vectorit = servers.begin(); vectorit != servers.end(); vectorit++)
			sendAll(ssl, (*vectorit)->socketfd, aux.c_str(), aux.length());
		return 0;
	}
	if (findUserInChannel(cli->nick_, it->second))
		return 1;
	//INVITES
	channelmodes = it->second->getModes();
	if (channelmodes.find('i') != std::string::npos)
	{
		for (invitesit = it->second->invites_.begin(); invitesit != it->second->invites_.end(); invitesit++)
		{
			if (*invitesit == cli->nick_)
			{
				isInvited = true;
				it->second->invites_.erase(invitesit);
				break ;
			}
		}
		if (!isInvited)
		{
			aux = replies.inviteonlychanReply(cli->nick_, it->second->name_);
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
			return 1;
		}
	}
	it->second->addClient(cli);
	it->second->clientsmode[cli->nick_];
	if (!it->second->topic_.empty())
	{
		aux = replies.topicReply(cli->nick_, it->second);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	}
	aux.clear();
	aux = replies.joinReply(cli, it->second->name_);
	sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	broadcastRest(it->second->clients, cli->socketfd, aux);
	aux.clear();
	aux = replies.namesReply(cli->nick_, it->second);
	sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	aux.clear();
	aux = replies.endnamesReply(cli->nick_, it->second->name_);
	sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	aux.clear();
	aux = replies.joinForward(cli->nick_, parser[1]);
	for (vectorit = servers.begin(); vectorit != servers.end(); vectorit++)
		sendAll(ssl, (*vectorit)->socketfd, aux.c_str(), aux.length());
	return 0;
}

int		Server::handle_part(std::string message, Client *cli)
{
	std::vector<std::string>					parser;
	std::map<std::string, Channel*>::iterator	it;
	std::string									aux;
	std::map<std::string, int>::iterator		statsit;
	std::vector<Client*>::iterator				cliit;

	statsit = server_stats.find("part");
	statsit->second += 1;

	if (cli->rol_ == "server")
		return (forward_part(message, cli));

	if (!(ft::checkMode("r", cli->mode_)))
	{
		aux = replies.notRegistered();
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}

	ft::split(message, parser);

	if (parser.size() == 1 || parser[1].size() == 0)
	{
		aux = replies.needMoreParams("PART");
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	if (parser[1][0] != '#')
	{
		aux = replies.nosuchchannelReply(cli->nick_, parser[1]);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	std::transform(parser[1].begin(), parser[1].end(), parser[1].begin(), ::tolower);
	if ((it = channels.find(parser[1])) == channels.end())
	{
		aux = replies.nosuchchannelReply(cli->nick_, parser[1]);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	if (!(findUserInChannel(cli->nick_, it->second)))
	{
		aux = replies.notonchannelReply(cli->nick_, parser[1]);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	std::vector<Client*>::iterator client_it;
	for(client_it = it->second->clients.begin(); client_it != it->second->clients.end(); ++client_it)
	{
		if((*client_it)->nick_ == cli->nick_)
		{
			aux = replies.partReply(cli, it->second->name_);
			it->second->clients.erase(client_it);
			break ;
		}
	}
	sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	broadcastRest(it->second->clients, cli->socketfd, aux);
	aux = replies.partForward(cli->nick_, it->second->name_, "leave channel");
	for (cliit = servers.begin(); cliit != servers.end(); cliit++)
		sendAll((*cliit)->ssl, (*cliit)->socketfd, aux.c_str(), aux.length());

	if(it->second->clients.size() == 0)
	{
		delete it->second;
		channels.erase(it);
	}
		
		
	return 0;
}

int		Server::handle_topic(std::string message, Client *cli)
{
	std::vector<std::string>						parser;
	std::map<std::string, Channel*>::iterator		it;
	std::map<std::string, std::string>::iterator	modesit;
	std::string										aux;
	std::string										modes;
	std::map<std::string, int>::iterator			statsit;

	statsit = server_stats.find("topic");
	statsit->second += 1;

	if (!(ft::checkMode("r", cli->mode_)))
	{
		aux = replies.notRegistered();
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}

	ft::split(message, parser);
	if (parser.size() == 1 || parser[1].size() == 0)
	{
		aux = replies.needMoreParams("TOPIC");
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	std::transform(parser[1].begin(), parser[1].end(), parser[1].begin(), ::tolower);
	if ((it = channels.find(parser[1])) == channels.end())
	{
		aux = replies.nosuchchannelReply(cli->nick_, parser[1]);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	modes = it->second->getModes();
	if (!(findUserInChannel(cli->nick_, it->second)) && (modes.find('s') != std::string::npos))
	{
		aux = replies.notonchannelReply(cli->nick_, it->second->name_);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	if (parser.size() == 2 || parser[2].size() == 0)
	{
		if (it->second->topic_.empty() || it->second->topic_ == " ")
		{
			aux = replies.notopicReply(cli->nick_, it->second->name_);
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
			return 1;
		}
		else
		{
			aux = replies.topicReply(cli->nick_, it->second->name_, it->second->topic_);
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
			return 0;
		}
	}
	else
	{
		if (parser.size() >= 3 && parser[2][0] != ':')
			parser[2].insert(0, ":") ;
		message.clear();
		for (size_t i = 0; i < parser.size(); i++)
		{
			message.append(parser[i]);
			message.append(" ");
		}
		parser.clear();
		ft::split(message, parser, ':');
		//IMPLEMENTAR MODOS
		if (!(findUserInChannel(cli->nick_, it->second)))
		{
			aux = replies.notonchannelReply(cli->nick_, it->second->name_);
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
			return 1;
		}
		else if (findUserInChannel(cli->nick_, it->second) && modes.find('t') == std::string::npos)
		{
			it->second->setTopic(parser[1]);
			aux = replies.settopicReply(cli, it->second->name_, it->second->topic_);
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
			broadcastRest(it->second->clients, cli->socketfd, aux);
		}
		else if (modes.find('t') != std::string::npos)
		{
			modesit = it->second->clientsmode.find(cli->nick_);
			if (isOperator(modesit->second))
			{
				it->second->setTopic(parser[1]);
				aux = replies.settopicReply(cli, it->second->name_, it->second->topic_);
				sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
				broadcastRest(it->second->clients, cli->socketfd, aux);
				return 0;
			}
			aux = replies.chanoprivsneededReply(cli->nick_, it->second->name_);
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
			return 1;
		}
	}
	return 0;
}

int		Server::handle_oper(std::string message, Client *cli)
{
	std::vector<std::string>				parser;
	std::string								aux;
	std::map<std::string, int>::iterator	statsit;

	statsit = server_stats.find("oper");
	statsit->second += 1;

	if (!(ft::checkMode("r", cli->mode_)))
	{
		aux = replies.notRegistered();
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}

	ft::split(message, parser);
	if (parser.size() < 3 || parser[2].size() == 0)
	{
		aux = replies.needMoreParams("OPER");
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	if (parser[1] != cli->nick_)
		return 1;
	else if (parser[2] != passw)
	{
		aux = replies.passwordmismatchReply(cli->nick_);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	else
	{
		cli->setMode("o", '+', cli->mode_);
		aux = replies.youareoperReply(cli->nick_);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	}
	return 0;
}

int		Server::handle_mode(std::string message, Client *cli)
{
	std::vector<std::string>						parser;
	std::string										aux;
	char											operation;
	std::string										usermodes;
	Channel											*newchannel;
	Client											*newclient;
	std::map<std::string, std::string>::iterator	mapit;
	std::map<std::string, int>::iterator			statsit;

	statsit = server_stats.find("mode");
	statsit->second += 1;

	if (!(ft::checkMode("r", cli->mode_)))
	{
		aux = replies.notRegistered();
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}

	ft::split(message, parser);
	if (parser.size() < 2 || parser[1].size() == 0)
	{
		aux = replies.needMoreParams("MODE");
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	//EL SEGUNDO ARGUMENTO ES UN CANAL
	if ((newchannel = channelExist(parser[1])) != nullptr)
	{
		//DOS ARGUMENTOS: DEVUELVE LOS MODOS DEL CANAL
		if (parser.size() == 2 || parser[2].size() == 0)
		{
			aux = replies.channelmodeisReply(cli->nick_, newchannel);
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
			return 0;
		}
		//TRES ARGUMENTOS: AÑADE MODOS AL CANAL
		else if (parser.size() == 3 || parser[3].size() == 0)
		{
			//COMPRUEBA SI ES OPERADOR DEL CANAL
			mapit = newchannel->clientsmode.find(cli->nick_);
			if (!(isOperator(mapit->second)))
			{
				aux = replies.chanoprivsneededReply(cli->nick_, newchannel->name_);
				sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
				return 1;
			}
			else
			{
				operation = parser[2][0];
				if (operation != '+' && operation != '-')
				{
					aux = replies.unknownmodeReply(cli->nick_, parser[2], newchannel->name_);
					sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
					return 1;
				}
				parser[2].erase(0, 1);
				for (size_t i = 0; i < parser[2].length(); i++)
				{
					aux.clear();
					if (parser[2][i] != 'm' && parser[2][i] != 't' && parser[2][i] != 's' && parser[2][i] != 'n' && parser[2][i] != 'i')
					{
						aux = replies.unknownmodeReply(cli->nick_, std::string(1, parser[2][i]), newchannel->name_);
						sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
						parser[2].erase(i, 1);
						i--;
					}
				}
				newchannel->channelSetMode(parser[2], operation, newchannel->mode_);
				aux = replies.channelmodeReply(cli, newchannel->name_, operation, parser[2]);
				sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
				broadcastRest(newchannel->clients, cli->socketfd, aux);
			}
		}
		//CUATRO ARGUMENTOS: AÑADE MODOS A LOS MIEMBROS DEL CANAL
		else if (parser.size() == 4 || parser[4].size() == 0)
		{
			//COMPRUEBA SI ES OPERADOR DEL CANAL
			mapit = newchannel->clientsmode.find(cli->nick_);
			if (!(isOperator(mapit->second)))
			{
				aux = replies.chanoprivsneededReply(cli->nick_, newchannel->name_);
				sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
				return 1;
			}
			else
			{
				if ((newclient = getClientByNick(parser[3])) == nullptr)
				{
					aux = replies.noSuchNick(cli->nick_, parser[3]);
					sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
					return 2;
				}
				if (!(findUserInChannel(newclient->nick_, newchannel)))
				{
					aux = replies.notuserinchannelReply(cli->nick_, parser[3], newchannel->name_);
					sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
					return 2;
				}
				operation = parser[2][0];
				parser[2].erase(0, 1);
				for (size_t i = 0; i < parser[2].length(); i++)
				{
					aux.clear();
					if (parser[2][i] != 'o' && parser[2][i] != 'v')
					{
						aux = replies.unknownmodeReply(cli->nick_, std::string(1, parser[2][i]), newchannel->name_);
						sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
						parser[2].erase(i, 1);
						i--;
					}
				}
				mapit = newchannel->clientsmode.find(newclient->nick_);
				newchannel->setMemberModes(parser[2], operation, mapit->second);				
				aux = replies.usermemberchannelmodeReply(cli, newchannel->name_, operation, newclient->nick_, parser[2]);
				sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
				broadcastRest(newchannel->clients, cli->socketfd, aux);
			}
		}
	}
	//EL SEGUNDO ARGUMENTO ES UN CLIENTE
	else if ((newclient = getClientByNick(cli->nick_)) != nullptr)
	{
		if (parser[1] != cli->nick_)
		{
			aux = replies.userdontmatchReply(cli->nick_);
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
			return 1;
		}
		if (parser.size() == 2 || parser[2].size() == 0)
		{
			aux = replies.umodeReply(cli);
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
			return 0;
		}
		operation = parser[2][0];
		parser[2].erase(0, 1);
		for (size_t i = 0; i < parser[2].length(); i++)
		{
			aux.clear();
			if (parser[2][i] != 'o' && parser[2][i] != 'a' && parser[2][i] != 'r' && parser[2][i] != 'i')
			{
				aux = replies.modeunknownflagReply(cli->nick_);
				sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
				parser[2].erase(i, 1);
			}
			else if((parser[2][i] == 'o' && operation == '+') || parser[2][i] == 'r' || parser[2][i] == 'a')
				parser[2].erase(i, 1);
		}
		cli->setMode(parser[2], operation, cli->mode_);
		aux = replies.modeReply(cli, operation, parser[2]);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	}
	//NO ES UN CANAL NI UN CLIENTE
	else
	{
		aux = replies.nosuchchannelReply(cli->nick_, parser[1]);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	return 0;
}

int		Server::handle_names(std::string message, Client *cli)
{
	std::vector<std::string>					parser;
	std::string									aux;
	std::map<std::string, Channel*>::iterator	it;
	std::map<std::string, int>::iterator		statsit;

	statsit = server_stats.find("names");
	statsit->second += 1;


	if (!(ft::checkMode("r", cli->mode_)))
	{
		aux = replies.notRegistered();
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}

	ft::split(message, parser);
	//Solo un parametro devuelve el listado de canales públicos y sus usuarios no invisibles, a menos que
	//el solicitante este en ese canal concreto
	if (parser.size() == 1 || parser[1].size() == 0)
	{
		for (it = channels.begin(); it != channels.end(); it++)
		{
			if (!(ft::checkMode("s", it->second->mode_)))
			{
				aux.clear();
				if (findUserInChannel(cli->nick_, it->second))
					aux = replies.namesinchannelReply(cli->nick_, it->second);
				else
					aux = replies.namesReply(cli->nick_, it->second);
				sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
			}
		}
	}
	//Devuelve la lista para el canal solicitado si es público y para los secretos si el solicitante
	//es parte del canal
	else if (parser.size() == 2 || parser[2].size() == 0)
	{
		it = channels.find(parser[1]);
		if (it != channels.end())
		{
			if (!(ft::checkMode("s", it->second->mode_)))
			{
				if (findUserInChannel(cli->nick_, it->second))
					aux = replies.namesinchannelReply(cli->nick_, it->second);
				else
					aux = replies.namesReply(cli->nick_, it->second);
				sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
			}
			else if (ft::checkMode("s", it->second->mode_) && findUserInChannel(cli->nick_, it->second))
			{
				aux = replies.namesinchannelReply(cli->nick_, it->second);
				sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
			}
		}
	}
	aux.clear();
	aux = replies.endnamesReply(cli->nick_, "/NAMES");
	sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	return 0;
}

int		Server::handle_away(std::string message, Client *cli)
{
	std::vector<std::string>				parser;
	std::string								aux;
	std::map<std::string, int>::iterator	statsit;

	statsit = server_stats.find("away");
	statsit->second += 1;

	if (!(ft::checkMode("r", cli->mode_)))
	{
		aux = replies.notRegistered();
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}

	ft::split(message, parser, ':');
	if (parser.size() == 1 || parser[1].size() == 0)
	{
		cli->away.clear();
		cli->setMode("a", '-', cli->mode_);
		aux = replies.unawayReply(cli->nick_);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 0;
	}
	else
	{
		cli->setAway(parser[1]);
		cli->setMode("a", '+', cli->mode_);
		aux = replies.noawayReply(cli->nick_);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 0;
	}
	return 0;
}

int		Server::handle_list(std::string message, Client *cli)
{
	std::vector<std::string>					parser;
	std::string									aux;
	std::map<std::string, Channel*>::iterator	it;
	std::vector<Client*>::iterator				clientsit;
	std::string									channelmodes;
	std::string									clientmodes;
	int											numberclients;
	std::map<std::string, int>::iterator		statsit;

	statsit = server_stats.find("list");
	statsit->second += 1;

	if (!(ft::checkMode("r", cli->mode_)))
	{
		aux = replies.notRegistered();
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}

	ft::split(message, parser);
	if (parser.size() == 1 || parser[1].size() == 0)
	{
		it = channels.begin();
		for ( ; it != channels.end(); it++)
		{
			channelmodes = it->second->getModes();
			if (channelmodes.find('s') != std::string::npos)
				continue ;
			numberclients = 0;
			for (clientsit = it->second->clients.begin(); clientsit != it->second->clients.end(); clientsit++)
			{
				clientmodes = (*clientsit)->getModes();
				if (clientmodes.find('i') == std::string::npos)
					numberclients++;
			}
			aux = replies.listReply(cli->nick_, it->second, numberclients);
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		}
		aux.clear();
		aux = replies.endlistReply(cli->nick_);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 0;
	}
	else
	{
		it = channels.find(parser[1]);
		if (it == channels.end())
		{
			aux = replies.noSuchNick(cli->nick_, parser[1]);
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
			aux.clear();
			aux = replies.endlistReply(cli->nick_);
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
			return 1;
		}
		else
		{
			channelmodes = it->second->getModes();
			if (channelmodes.find('s') != std::string::npos)
			{
				aux = replies.endlistReply(cli->nick_);
				sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
				return 1;
			}
			numberclients = 0;
			for (clientsit = it->second->clients.begin(); clientsit != it->second->clients.end(); clientsit++)
			{
				clientmodes = (*clientsit)->getModes();
				if (clientmodes.find('i') != std::string::npos)
					numberclients++;
			}
			aux = replies.listReply(cli->nick_, it->second, numberclients);
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		}
		aux = replies.endlistReply(cli->nick_);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	}
	return 0;
}

int		Server::handle_who(std::string message, Client *cli)
{
	std::vector<std::string>					parser;
	std::string									aux;
	std::string									channelmodes;
	std::string									usermodes;
	Channel										*newchannel;
	std::vector<Client*>::iterator				cliit;
	std::vector<Client*>::iterator				channelit;
	Client										*newclient;
	std::map<std::string, int>::iterator		statsit;

	statsit = server_stats.find("who");
	statsit->second += 1;

	if (!(ft::checkMode("r", cli->mode_)))
	{
		aux = replies.notRegistered();
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}

	ft::split(message, parser);
	//SIN PARÁMETROS SACAR TODOS LOS USUARIOS NO INVISIBLES SIN CANAL COMÚN
	if (parser.size() == 1 || parser[1].size() == 0 || parser[1] == "*" || parser[1] == "0")
	{
		for (cliit = clients.begin();  cliit != clients.end(); cliit++)
		{
			if ((*cliit)->nick_ == cli->nick_)
				continue ;
			usermodes.clear();
			usermodes = (*cliit)->getModes();
			if (usermodes.find('i') != std::string::npos)
				continue ;
			aux = replies.whoReply(cli->nick_, *cliit, "*", "");
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		}
		aux.clear();
		aux = replies.endwhoReply(cli->nick_, "*");
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 0;
	}
	//DOS PARÁMETROS
	else
	{
		//EL CANAL EXISTE
		if ((newchannel = channelExist(parser[1])) != nullptr)
		{
			//ESTOY EN EL CANAL
			if (findUserInChannel(cli->nick_, newchannel))
			{
				for (cliit = newchannel->clients.begin(); cliit != newchannel->clients.end(); cliit++)
				{
					aux.clear();
					channelmodes = newchannel->getMemberModes((*cliit)->nick_);
					aux = replies.whoReply(cli->nick_, *cliit, newchannel->name_, channelmodes);
					sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
				}
			}
			//NO ESTOY EN EL CANAL
			else
			{
				for (cliit = newchannel->clients.begin(); cliit != newchannel->clients.end(); cliit++)
				{
					usermodes = (*cliit)->getModes();
					if (usermodes.find('i') == std::string::npos)
					{
						aux.clear();
						channelmodes = newchannel->getMemberModes((*cliit)->nick_);
						aux = replies.whoReply(cli->nick_, *cliit, newchannel->name_, channelmodes);
						sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
					}
				};
			}	
		}
		//EL USUARIO EXISTE
		else if ((newclient = getClientByNick(parser[1])) != nullptr)
		{
			aux = replies.whoReply(cli->nick_, newclient, parser[1], "");
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		}
		aux.clear();
		aux = replies.endwhoReply(cli->nick_, parser[1]);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 0;
	}
	return 0;
}

int			Server::handle_invite(std::string message, Client *cli)
{
	std::vector<std::string>				parser;
	std::string								aux;
	std::string								channelmembermodes;
	Client									*newclient;
	Channel									*newchannel;
	std::map<std::string, int>::iterator	statsit;

	statsit = server_stats.find("invite");
	statsit->second += 1;

	if (!(ft::checkMode("r", cli->mode_)))
	{
		aux = replies.notRegistered();
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}

	ft::split(message, parser);
	if (parser.size() < 3 || parser[3].size() == 0)
	{
		aux = replies.needMoreParams("INVITE");
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	if ((newclient = getClientByNick(parser[1])) == nullptr)
	{
		aux = replies.noSuchNick(cli->nick_, parser[1]);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	if ((newchannel = channelExist(parser[2])) == nullptr)
	{
		aux = replies.nosuchchannelReply(cli->nick_, parser[2]);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	if (!findUserInChannel(cli->nick_, newchannel))
	{
		aux = replies.notonchannelReply(cli->nick_, parser[2]);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	channelmembermodes = newchannel->getMemberModes(cli->nick_);
	if (channelmembermodes.find('o') == std::string::npos)
	{
		aux = replies.chanoprivsneededReply(cli->nick_, parser[2]);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	if (findUserInChannel(newclient->nick_, newchannel))
	{
		aux = replies.userinchannelReply(cli->nick_, newclient->nick_, newchannel->name_);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	newchannel->invites_.push_back(newclient->nick_);
	aux = replies.invitingReply(cli->nick_, newclient->nick_, newchannel->name_);
	sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	aux.clear();
	aux = replies.broadcastinvitingReply(cli, newclient->nick_, newchannel->name_);
	sendAll(ssl, newclient->socketfd, aux.c_str(), aux.length());
	channelmembermodes.clear();
	channelmembermodes = newclient->getModes();
	if (channelmembermodes.find('a') != std::string::npos)
	{
		aux = replies.awayReply(cli->nick_, newclient->nick_, newclient->away);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	}
	return 0;
}

int			Server::handle_kick(std::string message, Client *cli)
{
	std::vector<std::string>				parser;
	std::string								aux;
	std::string								channelmembermodes;
	std::string								kick_message;
	Client									*newclient;
	Channel									*newchannel;
	std::vector<Client*>::iterator			client_it;
	std::map<std::string, int>::iterator	statsit;

	statsit = server_stats.find("kick");
	statsit->second += 1;

	if (!(ft::checkMode("r", cli->mode_)))
	{
		aux = replies.notRegistered();
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}

	ft::split(message, parser, ':');
	if (parser.size() == 1 || parser[1].size() == 0)
		kick_message = cli->nick_;
	else
		kick_message = parser[2];

	parser.clear();
	ft::split(message, parser);
	if (parser.size() < 3)
	{
		aux = replies.needMoreParams("KICK");
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	if ((newchannel = channelExist(parser[1])) == nullptr)
	{
		aux = replies.nosuchchannelReply(cli->nick_, parser[2]);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	if ((newclient = getClientByNick(parser[2])) == nullptr)
	{
		aux = replies.noSuchNick(cli->nick_, parser[1]);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	if (!findUserInChannel(cli->nick_, newchannel))
	{
		aux = replies.notonchannelReply(cli->nick_, parser[1]);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	channelmembermodes = newchannel->getMemberModes(cli->nick_);
	if (channelmembermodes.find('o') == std::string::npos)
	{
		aux = replies.chanoprivsneededReply(cli->nick_, parser[2]);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	if (!findUserInChannel(newclient->nick_, newchannel))
	{
		aux = replies.notuserinchannelReply(cli->nick_, newclient->nick_, newchannel->name_);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	for(client_it = newchannel->clients.begin(); client_it != newchannel->clients.end(); ++client_it)
	{
		if((*client_it)->nick_ == newclient->nick_)
		{
			aux = replies.broadcastkickReply(cli, newclient->nick_, newchannel->name_, kick_message);
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
			broadcastRest(newchannel->clients, cli->socketfd, aux);
			newchannel->clients.erase(client_it);
			break ;
		}
	}
	return 0;
}

/*
** ----------------------- SERVER TO SERVER COMMANDS -----------------------
*/


int		Server::handle_pass(std::string message, Client *cli)
{
	std::vector<std::string>		parser;
	std::string						aux;

	if (cli->rol_ != "unknown")
	{
		aux = replies.alreadyRegistered();
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}

	ft::split(message, parser);
	if (parser[0][0] != ':')
	{
		if (parser.size() < 2)
		{
			aux = replies.needMoreParams("PASS");
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
			return 1;
		}
		if (parser[1] != this->passw)
		{
			aux = replies.badpasswordReply();
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
			return 1;
		}
		else
			cli->setRol("registered");
	}
	else if (parser[0][0] == ':')
	{
		if (parser.size() < 3)
		{
			aux = replies.needMoreParams("PASS");
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
			return 1;
		}
		if (parser[2] != this->passw)
		{
			aux = replies.badpasswordReply();
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
			return 1;
		}
		else
			cli->setRol("registered");
	}
	
	return 0;
}

int		Server::handle_server(std::string message, Client *cli)
{
	std::vector<std::string>					parser;
	std::string									aux;
	std::string									info;
	std::vector<Client*>::iterator				it;
	std::string									host;
	Client										*newclient;
	std::map<std::string, Channel*>::iterator	mapit;

	ft::split(message, parser, ':');
	if (parser.size() > 1)
		info = *(parser.end() - 1);

	parser.clear();
	ft::split(message, parser);
	if (parser[0][0] != ':')
	{
		if (parser.size() < 2)
			return 1;
		cli->setServername(parser[1]);
		cli->setInfo(info);
		cli->setRol("server");
		cli->setHopcount("1");
		servers.push_back(cli);
		aux = replies.passReply();
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		aux.clear();
		aux = replies.serverReply(this->server_info);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());

		aux.clear();
		/* aux = replies.multipleserverReply(cli); */
		for (it = servers.begin(); it != servers.end(); it++)
		{
			if ((*it)->socketfd != cli->socketfd)
			{
				aux = replies.multipleserverReply(*it);
				sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
			}
		}
		aux = replies.multipleserverReply(cli);
		for (it = servers.begin(); it != servers.end(); it++)
		{
			if ((*it)->hopcount_ == 1 && (*it)->socketfd != cli->socketfd)
				sendAll(ssl, (*it)->socketfd, aux.c_str(), aux.length());
		}
		aux.clear();
		for (it = clients.begin(); it != clients.end(); it++)
		{
			if ((*it)->rol_ == "user" /* && (*it)->servername_ == this->servername */)
			{
				aux = replies.sendallnicksReply(*it);
				sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
			}
		}
		aux.clear();
		for (mapit = channels.begin(); mapit != channels.end(); mapit++)
		{
			aux = replies.njoinReply(mapit->second);
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		}
	}
	else if(parser[0][0] == ':')
	{
		host = parser[0];
		host.erase(0, 1);
		if (cli->rol_ == "server" && host == parser[3])
		{
			aux = replies.alreadyRegistered();
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
			return 1;
		}
		if (cli->rol_ == "registered")
		{
			cli->setServername(parser[2]);
			cli->setHopcount(parser[3]);
			cli->setInfo(info);
			cli->setRol("server");
			servers.push_back(cli);
		}
		else
		{
			newclient = new Client();
			newclient->setServername(parser[2]);
			int auxnum = atoi(parser[3].c_str());
			auxnum++;
			newclient->setHopcount(std::to_string(auxnum));
			newclient->setInfo(info);
			newclient->setRol("server");
			servers.push_back(newclient);
			aux.clear();
			aux = replies.multipleserverReply(newclient, newclient->hopcount_);
			for (it = servers.begin(); it != servers.end(); it++)
			{
				if ((*it)->hopcount_ == 1 && (*it)->socketfd != cli->socketfd)
					sendAll(ssl, (*it)->socketfd, aux.c_str(), aux.length());
			}
		}
	}

	return 0;
}

int		Server::handle_connect(std::string message, Client *cli)
{
	std::vector<std::string>				parser;
	std::string								aux;
	struct addrinfo							hints;
	struct addrinfo							*result, *rp;
	int										s;
	int										clientfd;
	std::vector<Client*>::iterator			it;
	std::map<std::string, int>::iterator	statsit;

	statsit = server_stats.find("connect");
	statsit->second += 1;

	if (!(ft::checkMode("r", cli->mode_)))
	{
		aux = replies.notRegistered();
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}

	ft::split(message, parser);
	if (parser.size() < 3)
	{
		aux = replies.needMoreParams("CONNECT");
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	if (!isOperator(cli->getModes()))
	{
		aux = replies.errnoprivilegesReply();
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}

	std::memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((s = getaddrinfo(parser[1].c_str(), parser[2].c_str(), &hints, &result)) != 0)
	{
		std::cerr << "Error: " << strerror(s) << std::endl;
		return -1;
	}
	for (rp = result; rp != NULL; rp = rp->ai_next)
	{
		clientfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
		if (sfd == -1)
			continue ;
		if (connect(clientfd, rp->ai_addr, rp->ai_addrlen) == -1)
		{
			close(sfd);
			continue ;
		}
		break ;
	}
	freeaddrinfo(result);
	if (rp == NULL)
	{
		std::cerr << "Error: failed to connect" << std::endl;
		return -1;
	}
	FD_SET(clientfd, &master);
	if (clientfd > fdmax)
		fdmax = clientfd;
	fcntl(clientfd, F_SETFL, O_NONBLOCK);
	Client	*newclient = new Client(clientfd);
	clients.push_back(newclient);
	aux = replies.sendPass();
	sendAll(cli->ssl, clientfd, aux.c_str(), aux.length());
	aux.clear();
	aux = replies.sendServer(this->server_info);
	sendAll(cli->ssl, clientfd, aux.c_str(), aux.length());
	for (it = clients.begin(); it != clients.end(); it++)
	{
		if ((*it)->rol_ == "user" && (*it)->servername_ == this->servername)
		{
			aux = replies.sendallnicksReply(*it);
			sendAll(cli->ssl, clientfd, aux.c_str(), aux.length());
		}
	}
	return 0;
}

int		Server::handle_squit(std::string message, Client *cli)
{
	//:Trillian SQUIT cm22.eng.umd.edu :Server out of control
	std::vector<std::string>		parser;
	std::string						aux;
	std::string						info;
	std::string						toerase;
	std::vector<Client*>::iterator	it;
	std::vector<Client*>::iterator	ite;
	(void)cli;

	ft::split(message, parser, ':');
	info = *(parser.end() - 1);

	parser.clear();
	ft::split(message, parser);
	toerase = parser[2];
	deleteServer(parser[2]);
	for (it = clients.begin(), ite = clients.end(); it != ite; it++)
	{
		if ((*it)->servername_ == toerase)
			deleteClient((*it)->nick_);
	}
	message = replies.squitForward(parser[2], "Server out of control");
	for (it = servers.begin(); it != servers.end(); it++)
	{
		if ((*it)->hopcount_ == 1 && (*it)->socketfd != cli->socketfd)
			sendAll((*it)->ssl, (*it)->socketfd, message.c_str(), message.length());
	}
	std::cout << parser[2] << " closed connection :" << info << std::endl;
	return 0;
}

int		Server::handle_time(std::string message, Client *cli)
{
	//:orwell.freenode.net 391 jarvis80 orwell.freenode.net :Thursday April 22 2021 -- 18:24:35 +00:00
	(void)message;
	const char * weekday[] = { "Sunday", "Monday",
                             "Tuesday", "Wednesday",
                             "Thursday", "Friday", "Saturday"};
	const char * months[] = { "January", "February", "March", "April", "May", "June",
							"July", "August", "September", "October", "November", "December"};
	std::time_t t = std::time(0);
	std::tm* 	now = std::localtime(&t);
	std::string	message_time;
	std::string	aux;
	std::map<std::string, int>::iterator	statsit;

	statsit = server_stats.find("time");
	statsit->second += 1;

	if (!(ft::checkMode("r", cli->mode_)))
	{
		aux = replies.notRegistered();
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}

	message_time.append(":");
	message_time.append(weekday[now->tm_wday]);
	message_time.append(" ");
	message_time.append(months[now->tm_mon + 1]);
	message_time.append(" ");
	message_time.append(std::to_string(now->tm_mday));
	message_time.append(" ");
	message_time.append(std::to_string(now->tm_year + 1900));
	message_time.append(" -- ");
	message_time.append(std::to_string(now->tm_hour));
	message_time.append(":");
	message_time.append(std::to_string(now->tm_min));
	message_time.append(":");
	message_time.append(std::to_string(now->tm_sec));
	message_time.append(" +02:00\r\n");

	aux = replies.timeReply(cli->nick_, message_time);
	sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	return 0;
}

int			Server::handle_version(std::string message, Client *cli)
{
	(void)message;

	std::string								aux;
	std::map<std::string, int>::iterator	statsit;

	statsit = server_stats.find("version");
	statsit->second += 1;

	if (!(ft::checkMode("r", cli->mode_)))
	{
		aux = replies.notRegistered();
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}

	aux = replies.versionReply(cli->nick_);
	sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	aux.clear();
	aux = replies.supportedReply(cli->nick_);
	sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	return 0;
}

int			Server::handle_stats(std::string message, Client *cli)
{
	std::vector<std::string>				parser;
	std::string								aux;
	std::map<std::string, int>::iterator	statsit;

	statsit = server_stats.find("stats");
	statsit->second += 1;

	if (!(ft::checkMode("r", cli->mode_)))
	{
		aux = replies.notRegistered();
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	ft::split(message, parser);
	if (parser.size() < 2)
	{
		aux = replies.needMoreParams("STATS");
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	if (!isOperator(cli->getModes()))
	{
		aux = replies.errnoprivilegesReply();
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}

	if (parser[1] == "l")
	{
		aux = replies.statslReply(cli);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		aux = replies.statsendReply(cli->nick_, parser[1]);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	}
	else if (parser[1] == "m")
	{
		for (statsit = server_stats.begin(); statsit != server_stats.end(); statsit++)
		{
			if (statsit->second > 0)
			{
				aux = replies.statsmReply(cli->nick_, statsit->first, statsit->second);
				sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
			}
		}
		aux = replies.statsendReply(cli->nick_, parser[1]);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	}
	else if (parser[1] == "u")
	{
		aux = replies.statsuReply(cli->nick_, this->t);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		aux = replies.statsendReply(cli->nick_, parser[1]);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	}
	
	return 0;
}

int		Server::handle_trace(std::string message, Client *cli)
{
	std::vector<std::string>				parser;
	std::string								aux;
	std::map<std::string, int>::iterator	statsit;
	Client									*newclient;
	std::vector<Client*>::iterator			it;
	std::vector<Client*>::iterator			cliit;

	statsit = server_stats.find("trace");
	statsit->second += 1;

	if (!(ft::checkMode("r", cli->mode_)))
	{
		aux = replies.notRegistered();
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	ft::split(message, parser);
	if (parser.size() == 1)
	{
		aux = replies.traceReply(cli->nick_, cli);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		aux = replies.traceendReply(cli->nick_, cli);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	}
	else
	{
		newclient = getClientByNick(parser[1]);
		if (newclient)
		{
			aux = replies.traceReply(cli->nick_, newclient);
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
			aux = replies.traceendReply(cli->nick_, newclient);
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
			return 0;
		}
		else
		{
			for (it = servers.begin(); it != servers.end(); it++)
			{
				if ((*it)->servername_ == parser[1])
				{
					for (cliit = clients.begin(); cliit != clients.end(); cliit++)
					{
						if ((*cliit)->servername_ == parser[1] && (*cliit)->rol_ == "user")
						{
							aux = replies.traceReply(cli->nick_, *cliit);
							sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
						}
					}
					aux = replies.traceendReply(cli->nick_, cli);
					sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
					return 0;
				}
			}
			aux = replies.noSuchServer(cli->nick_, parser[1]);
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		}
	}
	return 0;
}

int			Server::handle_links(std::string message, Client *cli)
{
	std::string						aux;
	std::vector<Client*>::iterator	it;
	std::vector<std::string>		parser;
	Client							*servquery;
	ft::split(message, parser);
	if (!(ft::checkMode("r", cli->mode_)))
	{
		aux = replies.notRegistered();
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}
	if(parser.size() == 1 || parser[1].size() == 0)
	{
		for(it = servers.begin(); it != servers.end(); it++)
		{
			if((*it)->hopcount_ == 1)
			{
				aux = replies.linksReply(cli->nick_, (*it));
				sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
			}
		}
		aux.clear();
		aux = replies.linksownservReply(cli->nick_);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		aux.clear();
		aux = replies.endoflinksReply(cli->nick_, "*");
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	}
	else if(parser.size() == 2)
	{
		for(it = servers.begin(); it != servers.end(); it++)
		{
			if((*it)->servername_ == parser[1])
				servquery = (*it);
		}
		if(servquery->servername_ == parser[1])
		{
			aux = replies.linksReply(cli->nick_, servquery);
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		}
		if(parser[1] == servername)
		{
			aux = replies.linksownservReply(cli->nick_);
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		}
		aux.clear();
		aux = replies.endoflinksReply(cli->nick_, parser[1]);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	}
	else if(parser.size() > 2)
	{
		aux = replies.linksownservReply(cli->nick_);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		aux = replies.endoflinksReply(cli->nick_, "*");
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	}
	return 0;
}
int		Server::handle_info(std::string message, Client *cli)
{
	(void)message;
	std::string		aux;
	aux = replies.infoReply(cli->nick_, t);
	sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	aux.clear();
	aux = replies.endofInfoReply(cli->nick_);
	sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	return 0;
}

int		Server::handle_admin(std::string message, Client *cli)
{
	std::string 					aux;
	std::vector<std::string>		parser;

	ft::split(message, parser);

	if(parser.size() == 1)
	{
		aux = replies.adminReply(cli->nick_);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
	}
	else if(parser.size() == 2)
	{
		if(findUserInServer(parser[1]))
		{
			aux = replies.adminReply(cli->nick_);
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		}
		else
		{
			aux = replies.noSuchServer(cli->nick_, parser[1]);
			sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
			return 1;
		}
	}
	else if(parser.size() > 2)
	{
		aux = replies.noSuchServer(cli->nick_, parser[1]);
		sendAll(cli->ssl, cli->socketfd, aux.c_str(), aux.length());
		return 1;
	}

	return 0;
}

int			Server::handle_njoin(std::string message, Client *cli)
{
	(void)cli;
	std::vector<std::string>			parser;
	std::vector<std::string>			parser1;
	std::string							users;
	std::string							channame;
	Client								*newclient;
	Channel								*newchannel;
	std::vector<std::string>::iterator	it;

	ft::split(message, parser, ':');
	users = *(parser.end() - 1);
	ft::split(users, parser1, ',');
	parser.clear();
	ft::split(message, parser);
	channame  = parser[2];
	newchannel = new Channel(channame);
	std::pair<std::string, Channel*> pair;
	pair = std::make_pair(channame, newchannel);
	channels.insert(pair);
	for (it = parser1.begin(); it != parser1.end(); it++)
	{
		if ((*it)[0] == '@')
		{
			(*it).erase(0, 1);
			newclient = getClientByNick(*it);
			newchannel->clientsmode[newclient->nick_] = "o";
			newchannel->clients.push_back(newclient);
		}
		else
		{
			newclient = getClientByNick(*it);
			newchannel->clientsmode[newclient->nick_];
			newchannel->clients.push_back(newclient);
		}
	}
	return 0;
}

/* ************************************************************************** */