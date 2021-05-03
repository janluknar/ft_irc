# ft_irc
Implementation of an almost fully fledged IRC server following the RFC standards. Made alongside @dalba-de.
# How to Use
Compile on Linux or MacOS with ```make``` and execute with the following arguments: ```./ircserv [host:port_network:password_network] <port> <password>```.
If you want to connect with a TLS connection you must generate your own certificate and private keys with ```openssl```. Just save them as ```server.crt``` and ```server.key```.
You can connect using any IRC client you want.
