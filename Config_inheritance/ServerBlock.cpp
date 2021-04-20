/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerBlock.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 09:37:57 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/20 19:58:18 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerBlock.hpp"

ServerBlock::ServerBlock(void) :
_listenIP(DEFAULT_LISTEN_IP),
_listenPort(DEFAULT_LISTEN_PORT),
_server_names(1, DEFAULT_SRV_NAME)
{
	// std::cout << GREEN << "SERVER CONSTRUCTOR" << NOCOLOR <<std::endl;
	// this->_listenIP = DEFAULT_LISTEN_IP;
	// this->_listenPort = DEFAULT_LISTEN_PORT;
	// this->_root.clear();
	// this->_autoindex = NOT_SET;
	// this->_indexes.clear();
	// this->_client_max_body_size = NOT_SET;
	// this->_keepalive_timeout = NOT_SET;
	// this->_chunked_transfer_encoding = NOT_SET;
	// this->_auth_basic = DEFAULT_AUTH_BASIC;
	// this->_server_names.push_back(DEFAULT_SRV_NAME);


	return ;
}

ServerBlock::ServerBlock(ServerBlock const & copy)
{
	// std::cout << YELLOW << "SERVER CPY CONSTRUCTOR" << NOCOLOR <<std::endl;
	this->_listenIP = copy._listenIP;
	this->_listenPort = copy._listenPort;
	this->_server_names = copy._server_names;
	this->_root = copy._root;
	this->_autoindex = copy._autoindex;
	this->_indexes = copy._indexes;
	this->_limit_except = copy._limit_except;
	this->_error_pages = copy._error_pages;
	this->_client_max_body_size = copy._client_max_body_size;
	this->_keepalive_timeout = copy._keepalive_timeout;
	this->_chunked_transfer_encoding = copy._chunked_transfer_encoding;
	this->_auth_basic = copy._auth_basic;
	this->_auth_basic_user_file = copy._auth_basic_user_file;
	this->_locations = copy._locations;

	return ;
}

ServerBlock::~ServerBlock(void)
{
	return ;
}

ServerBlock& ServerBlock::operator=(ServerBlock const & rhs)
{
	this->_listenIP = rhs._listenIP;
	this->_listenPort = rhs._listenPort;
	this->_server_names = rhs._server_names;
	this->_root = rhs._root;
	this->_autoindex = rhs._autoindex;
	this->_indexes = rhs._indexes;
	this->_limit_except = rhs._limit_except;
	this->_error_pages = rhs._error_pages;
	this->_client_max_body_size = rhs._client_max_body_size;
	this->_keepalive_timeout = rhs._keepalive_timeout;
	this->_chunked_transfer_encoding = rhs._chunked_transfer_encoding;
	this->_auth_basic = rhs._auth_basic;
	this->_auth_basic_user_file = rhs._auth_basic_user_file;
	this->_locations = rhs._locations;

	// std::cout << RED << "SERVER ASSIGNATION" << NOCOLOR <<std::endl;
	return *this;
}


// Setters

int	ServerBlock::setListenIp(std::string ip)
{
	if (ip == "localhost")
		ip = "127.0.0.1";
	if (ip.find_first_not_of("0123456789.") == std::string::npos && !ip.empty())
	{
		this->_listenIP = stringToIp(ip);
		return SUCCESS;
	}
	return FAILURE;
}

int	ServerBlock::setListenPort(std::string port)
{
	if (ft_isdigit_string(port))
	{
		this->_listenPort = atoi(port.c_str());
		if (this->_listenPort != 0 && this->_listenPort <= MAX_TCP_PORT)
			return (SUCCESS);
	}
	return FAILURE;
}

void	ServerBlock::setServerNames(strVecIterator first, strVecIterator last)
{
	this->_server_names.assign(first, last);
}


// Getters

const unsigned int&		ServerBlock::getListenIP(void) const
{
	return this->_listenIP;
}

const size_type&		ServerBlock::getListenPort(void) const
{
	return this->_listenPort;
}

const stringVec&		ServerBlock::getServerNames(void) const
{
	return this->_server_names;
}

const LocMap&	ServerBlock::getLocations(void) const
{
	return this->_locations;
}

std::pair<LocMap::iterator,bool>	 ServerBlock::addLocation(std::string path)
{
	LocationBlock loc;
	return this->_locations.insert(std::make_pair(path, loc));
}

std::ostream & operator<<(std::ostream & o, ServerBlock const & rhs)
{
	static int i = 0;
	static std::string pad("  ");

	o << std::endl << "=> SERVER N°" << ++i << std::endl;
	o << pad << "------------" << std::endl;

	o << pad << "LISTEN IP: " << ipToString(rhs.getListenIP()) << std::endl;
	o << pad << "LISTEN PORT: " << rhs.getListenPort() << std::endl;
	
	o << pad << "SERVER NAMES: ";
	putVecToOstream(o, rhs.getServerNames().begin(), rhs.getServerNames().end());

	o << pad << "ROOT: " << rhs.getRoot() << std::endl;

	o << pad << "INDEXES: ";
	putVecToOstream(o, rhs.getIndexes().begin(), rhs.getIndexes().end());

	o << pad << "AUTOINDEX: "; putState(o, rhs.getAutoindex());
	
	o << pad << "LIMIT_EXCEPT: ";
	putVecToOstream(o, rhs.getLimitExcept().begin(), rhs.getLimitExcept().end());

	o << pad << "ERROR PAGES: ";
	putMapToOstream(o, rhs.getErrorPages().begin(), rhs.getErrorPages().end());
	
	o << pad << "MAX BDY SIZE: " << rhs.getMaxBdySize() << std::endl;
	o << pad << "KEEP. TIMEOUT: " << rhs.getKeepaliveTime() << std::endl;
	
	o << pad << "CHUNKED ENC.: "; putState(o, rhs.getChunkedEncoding());
	
	o << pad << "AUTH BASIC: " << rhs.getAuthBasic() << std::endl;
	o << pad << "AUTH BASIC FILE: " << rhs.getAuthBasicFile() << std::endl;

	// Locations
	putLocToOstream(o, rhs.getLocations().begin(), rhs.getLocations().end());

	return o;
}
