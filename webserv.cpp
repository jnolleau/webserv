#include "Server.hpp"
#include "ConfParser.hpp"
#include "HttpBlock.hpp"
#include <csignal> // To handle CTRL-C (and others signals ?)

void signal_handler(int signum)
{
	if (signum == SIGINT)
	{
		Server::server_is_alive = 0;
		std::cerr << std::endl;
	}
}

LocationBlock match_loc(std::string target_uri, LocMap locations)
{
    while (target_uri.find('/') != std::string::npos)
    {
        target_uri = target_uri.substr(0, target_uri.find_last_of('/')); // cut target_uri at last '/'
        for (LocMap::const_iterator it = locations.begin(); it != locations.end(); it++)
        {
            if (target_uri.compare(it->first) == 0)
                return it->second;
        }
    }
    // if no match, return "empty" location block
    LocationBlock loc;
    return loc;
}

int main(int ac, char **av)
{
	if (signal(SIGINT, signal_handler) == SIG_ERR)
		return (1);

	if (ac == 2)
	{
		ConfParser parser;

		try {
			parser.readConfFile(DEFAULT_CONF_FILE); // Default config file containing http directive
			parser.readConfFile(av[1]); // Config file containing server blocks

			HttpBlock baseConfig = parser.getHttpBlock();
			std::vector<ServerBlock> servers = parser.getServers();

			// std::cout << "matched loc: " << match_loc("/lost/machin/chose", servers[0].getLocations()) << std::endl;
			// std::cout << "matched loc: " << match_loc("/last/prout", servers[0].getLocations()) << std::endl;
			// std::cout << "matched loc: " << match_loc("/last/prout/", servers[0].getLocations()) << std::endl;
			std::cout << "matched loc: " << match_loc("/lisst/prout", servers[0].getLocations()) << std::endl;
/*			std::cout << std::endl << "WEBSERV CONFIGURATION: " << std::endl;
			std::cout << "Nbr of servers: " << servers.size() << std::endl << std::endl;
			
			std::cout << baseConfig;
			displayVec(servers, '\n');
*/		
			/*Create and aunch server*/
			Server *server =  new Server(baseConfig, servers);
			server->launch();
			delete server;
		}
		catch(const std::exception& e) {
			std::cerr << e.what() << '\n';
		}
	}


	return (0);
}
