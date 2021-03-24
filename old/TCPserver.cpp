#include "webserv.hpp"

int main() {
	// Create a socket (IPv4, TCP)
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		std::cout << "Failed to create socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}

	// Listen to port 9999 on any address
	sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons(9999); // htons is necessary to convert a number to
																	 // network byte order
	if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0) {
		std::cout << "Failed to bind to port 9999. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
// Start listening. Hold at most 10 connections in the queue
	if (listen(sockfd, 10) < 0) {
		std::cout << "Failed to listen on socket. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}
	// Grab a connection from the queue
	// Will block until a request arrives
	unsigned long addrlen = sizeof(sockaddr);
	int connection = accept(sockfd, (struct sockaddr*)&sockaddr, (socklen_t*)&addrlen);
	if (connection < 0) {
		std::cout << "Failed to grab connection. errno: " << errno << std::endl;
		exit(EXIT_FAILURE);
	}

    // process requests until an error is found on a request
		std::string request_ok = "Request received :)\n";
		std::string request_ko = "Request error :( \n";
		while (1)
		{
			Request *req = new Request;
    	process_request(connection, *req);
			if ((*req).get_error_code())
				send(connection, request_ko.c_str(), request_ko.size(), 0);
			else
				send(connection, request_ok.c_str(), request_ok.size(), 0);
			(*req).print();
			if ((*req).get_error_code())
			{
				delete req;
				break;
			}
			delete req;
		}
		
	
	// Send a message to the connection

    // // Send content of a file
	// 	std::ifstream file;
	// 	file.open("old/nginx.conf");
	// 	if (file.fail())
	// 	{
	// 			std::cout << "cant open file" << std::endl;
	// 	}
	// 	std::string line;
	// 	while (getline(file, line))
	// 	{
	// 			send(connection, line.c_str(), line.size(), 0);
	// 			send(connection, "\n", 1, 0);
	// 	}

    // send simple string
	std::string response = "Good talking to you\n";
	send(connection, response.c_str(), response.size(), 0);

	// Close the connections
	close(connection);
	close(sockfd);
}