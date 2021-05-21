#include "Server.hpp"

/*Server::Server(void)
{
	return ;
}
*/
Server::Server(HttpBlock const& config, std::vector<ServerBlock> const& srvs)
: baseConfig(config), servers(srvs)
{
	return ;
}

/*Server::Server(Server const & copy)
{
	return ;
}
*/
Server::~Server(void)
{
	std::map<int, Request*>::iterator end = this->requests.end();
	
	// Clean up all of the sockets that are open 
	for (std::map<int, Request*>::iterator it = this->requests.begin(); it != end;)
	{
		if (FD_ISSET(it->first, &this->ready_sockets))
		{
			this->close_socket(it++);
		}
		else
			std::cerr << "fd " << it->first << "is still in use." << std::endl;;
	}

	sockMap::const_iterator end2 = this->server_sockets.end();
	for (sockMap::const_iterator it = this->server_sockets.begin(); it != end2; ++it)
	{
		if (close(it->first) < 0)
		{
			std::cerr << "Failed to close. errno:" << errno << std::endl;
			exit(EXIT_FAILURE);
		}
		std::cerr << WHITE << "server_socket closed (fd " << it->first << ")."
		<< NOCOLOR <<std::endl;
	}

	return ;
}

int	Server::server_is_alive = 1;


int Server::launch(void)
{
	fd_set current_sockets;
	int rdy_fd = 0;
	int client_socket;

	// Create vector of server_sockets, bind and listen
	if (this->setup() < 0)
		return (FAILURE);

	//Initilaize the current socket set
	FD_ZERO(&current_sockets);

	/* Adds all servers socket in socket SET */
	sockMap::const_iterator end = this->server_sockets.end();
	for (sockMap::const_iterator it = this->server_sockets.begin(); it != end; ++it)
	{
		FD_SET(it->first, &current_sockets);
	}
	
	// For Select (it needs the highest-numbered file descriptor in any of the three sets, plus 1)
	int max_socket = this->server_sockets.rbegin()->first;
	std::cerr << "MAX SOCKET: " << max_socket << std::endl;

	//Optional: sets the timeout for select()
	struct timeval timeout;

	while (Server::server_is_alive)
	{
		//Optional: sets the timeout for select()
		// Il s'agit du temps le plus long que select() pourrait attendre avant de rendre la main, même si rien d'intéressant n'est arrivé.
		// Si cette valeur est positionnée à NULL, alors, select() bloque indéfiniment dans l'attente qu'un descripteur de fichier devienne prêt
		timeout.tv_sec = 5;
		timeout.tv_usec = 0;

		// select is destructive so we need a socket copy
		this->ready_sockets = current_sockets;
		int sret = select(max_socket + 1, &this->ready_sockets, NULL, NULL, NULL);
		// suite à l'appel à select, this->ready_sockets ne contient plus que les fd disponibles en lecture
		// (current_sockets contient toujours tous les sockets existants)

		// En cas de succès, select() renvoie le nombre total de descripteurs de fichiers encore présents dans les ensembles de descripteurs de fichier.
		// En cas de timeout échu, alors les fd_set devraient tous être vides -> la valeur renvoyée est zéro
		// En cas d'erreur, renvoie -1	
		if (sret < 0 && errno != EINTR)
		{
			std::cerr << "Select error. errno: " << errno << std::endl;
			return (FAILURE);
		}
		else if (sret == 0)
			std::cerr << sret << ": Select timeout (5 sec)" << std::endl;

		// On parcourt nos fd (client sockets et server sockets): pour chaque fd on teste s'il est présent dans this->ready_sockets = disponible en lecture
		rdy_fd = sret;

		// 1. on parcourt d'abord les server_sockets. S'ils sont disponibles en lecture, c'est qu'une nouvelle connection est etablie -> accept connection et creation d'un client socket
		for (sockMap::const_iterator it = this->server_sockets.begin(); it != end; ++it)
		{
			if (FD_ISSET(it->first, &this->ready_sockets))
			{
				rdy_fd--;
				client_socket = this->accept_new_connection(it->first);
				if (client_socket < 0 && errno != EAGAIN) 
				{
					std::cout << "Failed to grab connection. errno: " << errno << std::endl;
					return (FAILURE);
				}
				else if (client_socket > 0)
				{
					std::cout << YELLOW << "New incoming connection (fd " << client_socket << ")" << NOCOLOR << std::endl;
					FD_SET(client_socket, &current_sockets); // new connection is added to fd_set (current socket)
					
					Request *req = new Request(client_socket, it->second, this->servers, this->baseConfig);
					this->requests.insert(std::make_pair(client_socket, req));

					if (client_socket > max_socket)
						max_socket = client_socket;
				}
			}
		}
		// 2. on parcourt ensuite les client_sockets. S'ils sont disponibles en lecture, c'est qu'on peut parser une requete (rq: une meme requete peut etre envoyee en plusieurs fois)
		std::map<int, Request*>::iterator end = this->requests.end();
		for (std::map<int, Request*>::iterator it = this->requests.begin(); it != end && rdy_fd > 0;)
		{
			if (FD_ISSET(it->first, &this->ready_sockets))
			{
				std::cout << GREEN << "Communication with client -> fd " << it->first << NOCOLOR << std::endl;
				
				// Parse the request
				it->second->parse();
				it->second->print();
				if (it->second->connection_end() || it->second->get_error_code())
				{
					// log message
					if (it->second->connection_end())
						std::cout << RED << "Client closed connection" << NOCOLOR << std::endl;
					else
						std::cout << RED << "Request error, closing connection" << NOCOLOR << std::endl;

					// Remove client_socket from FD SET
					FD_CLR(it->first, &current_sockets);
					if (it->first == max_socket)
						max_socket--;
					this->close_socket(it++); // == copy it, increment it, then send the copy (non incremented) to close_socket
					continue;
				}
				if (it->second->request_is_ready())
				{
					std::cout << "Request is ready" << std::endl;
					// 1. match server_block and location block
					// 2. "execute" request based on config
					// 3. send response

					it->second->reset();
				}
			}
			it++;
		}
	}
	return 0;
}

bool my_comp(ServerBlock serv1, ServerBlock serv2)
{
	if (serv1.getListenIP() < serv2.getListenIP())
		return true;
	return false;
}

int Server::setup(void)
{
	// displayVec(this->servers, '\n');
	std::vector<ServerBlock> sorted_blocks(this->servers);
	std::sort(sorted_blocks.begin(), sorted_blocks.end(), my_comp);
	// for (std::vector<ServerBlock>::iterator it = sorted_blocks.begin(); it != sorted_blocks.end(); it++)
	// {
	// 	std::cout << "sorted block: " << ipToString(it->getListenIP()) << std::endl;
	// }

	std::vector<ServerBlock>::const_iterator it = sorted_blocks.begin();
	while (it != sorted_blocks.end())
	{
		// Listen to any addresses and port found in Config
		sockaddr_in sockaddr;
		
		memset((char *)&sockaddr, 0, sizeof(sockaddr)); 
		sockaddr.sin_family = AF_INET;
		sockaddr.sin_addr.s_addr = it->getListenIP(); // Adresse dans l'ordre d'octets réseau
		sockaddr.sin_port = it->getListenPort(); // htons is necessary to convert a number to
		std::cerr << "IP: " << ipToString(sockaddr.sin_addr.s_addr) << std::endl;
		std::cerr << "PORT: " << ntohs(it->getListenPort()) << std::endl;

		// Create a socket (IPv4, TCP)
		// Using the flag SOCK_NONBLOCK saves extra calls to fcntl(2) to achieve the same result.
		int newSocket;
		memset((char *)&newSocket, 0, sizeof(newSocket)); 
		newSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (newSocket == -1)
		{
			std::cout << "Failed to create socket. errno: " << errno << std::endl;
			return (FAILURE);
		}

		// Set the socket as NON BLOCKING (for MAC, no need in Linux ?)
		if (fcntl(newSocket, F_SETFL, O_NONBLOCK) < 0)
			std::cout << "Fcntl failed. errno: " << errno << std::endl;

		// In order to reuse the socket quickly after stopping and restarting the server
		// Don't take TIME_WAIT into consideration
		int optval = 1; // The optval sets to 1 (or > 0) enables teh OPTION, sets to 0 disable it
		if (setsockopt(newSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) < 0)
			std::cout << "Error: setsockopt(SO_REUSEADDR) failed. errno: " << errno << std::endl;

		if (bind(newSocket, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
		{
			close(newSocket);
			if (errno != EADDRINUSE)
			{
				std::cout << "Failed to bind to port " << ntohs(sockaddr.sin_port) << ". errno: " << errno << std::endl;
				return (FAILURE);
			}
		}
		else
		{
			// Start listening. Hold at most 10 connections in the queue
			if (listen(newSocket, 10) < 0)
			{
				std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
				close(newSocket);
				return (FAILURE);
			}
			this->server_sockets.insert(std::pair<int, sockaddr_in>(newSocket, sockaddr));
		}

		++it;
	}
	return (SUCCESS);
}

int Server::accept_new_connection(int server_socket)
{
	int	client_socket;

	client_socket = accept(server_socket, NULL, NULL);
	// if (client_socket < 0)
	// 	std::cout << "Failed to grab connection. errno: " << errno << std::endl;
	// else
	// {
	// 	if (fcntl(client_socket, F_SETFL, O_NONBLOCK) < 0)
	// 	std::cout << "Fcntl failed. errno: " << errno << std::endl;
	// }
	return (client_socket);
}

void Server::close_socket(std::map<int, Request*>::iterator it)
{
	if (close(it->first) < 0)
	{
		std::cerr << "Failed to close. errno:" << errno << std::endl;
		exit(EXIT_FAILURE);
	}
	std::cout << YELLOW << "Client " << it->first << " disconnected." << NOCOLOR << std::endl;

	// delete the Request 
	delete it->second;
	// Erase the map element containing the former request
	this->requests.erase(it->first);
}

