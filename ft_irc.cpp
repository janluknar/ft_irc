#include "Server.hpp"

int main(int argc, char **argv)
{
	Server serv(argc, argv);

	if (serv.init_server())
		exit(EXIT_FAILURE);
	if	(serv.init_ssl())
		exit(EXIT_FAILURE);
	if (argc == 4)
	{
		if (serv.connect_to_serv())
			std::cout << "Cannot connect to the other server" << std::endl;
	}
	serv.selectLoop();
	return 0;
}