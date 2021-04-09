/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/16 15:55:10 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/07 19:10:22 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfParser.hpp"
#include <stdio.h>
#include <cstring>

ConfParser::ConfParser(void)
: _configFile("nginx.conf"), _httpBlock(new HttpBlock()),
_block_type(HTTP), _line_nb(1), _nbr_of_srv(0),
_nbr_of_loc(0), _in_block(FALSE), _semi_col_not_found(0),
_curr_dir(), _dir_line(new std::vector<std::string>)
{
	return;
}

ConfParser::ConfParser(const std::string filename)
: _configFile(filename), _httpBlock(new HttpBlock()),
_block_type(HTTP), _line_nb(1), _nbr_of_srv(0),
_nbr_of_loc(0), _in_block(FALSE), _semi_col_not_found(0),
_curr_dir(), _dir_line(new std::vector<std::string>)
{
	return; 
}

ConfParser::~ConfParser(void)
{
	std::cout << "DESTRUCTOR CALLED" << std::endl;
	delete this->_httpBlock;
	delete this->_dir_line;
	return ;
}

/**
 * Static attributs
 *
 * Contains directive listing assiociated
 * with their function handler 
 */

typedef std::map<std::string, int (ConfParser::*)(std::string&)>	dirMap; //move this typedef in higher level ?

dirMap	ConfParser::http_map = setHttpMap();
dirMap	ConfParser::srv_map = setSrvMap();
dirMap	ConfParser::loc_map = setLocMap();

dirMap	ConfParser::setHttpMap()
{
	dirMap map;

	map["root"] = &ConfParser::setRoot;
	map["error_page"] = &ConfParser::setErrorPage;
	map["keepalive_timeout"] = &ConfParser::setTimeout;
	map["client_max_body_size"] = &ConfParser::setMaxBdySize;
	map["cgi_param"] = &ConfParser::setCgiParam;
	map["cgi_pass"] = &ConfParser::setCgiPass;
	map["allow_methods"] = &ConfParser::setAllowedMethods;
	map["index"] = &ConfParser::setIndex;
	map["autoindex"] = &ConfParser::setAutoIndex;

	return map;
}

dirMap	ConfParser::setSrvMap()
{
	dirMap map;

	map["listen"] = &ConfParser::setListen;
	map["root"] = &ConfParser::setRoot;
	map["server_name"] = &ConfParser::setServerName;
	map["error_page"] = &ConfParser::setErrorPage;
	map["cgi_param"] = &ConfParser::setCgiParam;
	map["cgi_pass"] = &ConfParser::setCgiPass;
	map["allow_methods"] = &ConfParser::setAllowedMethods;
	map["index"] = &ConfParser::setIndex;
	map["autoindex"] = &ConfParser::setAutoIndex;

	return map;
}

dirMap	ConfParser::setLocMap()
{
	dirMap map;

	map["listen"] = &ConfParser::setListen;
	map["root"] = &ConfParser::setRoot;
	map["error_page"] = &ConfParser::setErrorPage;
	map["cgi_param"] = &ConfParser::setCgiParam;
	map["cgi_pass"] = &ConfParser::setCgiPass;
	map["allow_methods"] = &ConfParser::setAllowedMethods;
	map["index"] = &ConfParser::setIndex;
	map["autoindex"] = &ConfParser::setAutoIndex;

	return map;
}

int ConfParser::setListen(std::string& token)
{
	(void)token;
	std::cout << "SET LISTEN FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setServerName(std::string& token)
{
	(void)token;
	std::cout << "SET SRVNAME FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setRoot(std::string& token)
{
	(void)token;
	std::cout << "SET ROOT FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setErrorPage(std::string& token)
{
	(void)token;
	std::cout << "SET ERROPAGE FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setTimeout(std::string& token)
{
	(void)token;
	std::cout << "SET TIME FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setMaxBdySize(std::string& token)
{
	(void)token;
	std::cout << "SET MAXBDYSIZE FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setCgiParam(std::string& token)
{
	(void)token;
	std::cout << "SET CGIPARAM FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setCgiPass(std::string& token)
{
	(void)token;
	std::cout << "SET CGIPASS FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setAllowedMethods(std::string& token)
{
	(void)token;
	std::cout << "SET ALLMETH FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setIndex(std::string& token)
{
	(void)token;
	std::cout << "SET INDX FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setAutoIndex(std::string& token)
{
	(void)token;
	std::cout << "SET AUTOIDX FUNCTION" << std::endl;
	return 0;
}

int ConfParser::readConfFile()
{
	std::ifstream	file;
	std::string		line;
	size_t			ret = 0;


	file.open(this->_configFile.c_str());
	if (file.fail())
	{
		std::cerr << "webserv: cannot open file \"" << this->_configFile << "\"" << std::endl;
		return 1;
	}
	try {
		while (getline(file, line))
		{
			ret = this->parseLine(line);
			
			file.clear();
			if (ret == 1)
				break;
			this->_line_nb++;
		}
		if (this->_in_block[HTTP] == TRUE)
			throw NoClosingBracket("http", this->_line_nb);
		else
		std::cout << GREEN << "FIIIIIIIIIINNNNNNNN" << NOCOLOR << std::endl;
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
		ret = 1;
	}
	return ret;
}

void erase_comments(std::string& line)
{
	size_t pos;
	size_t len = 0;

	pos = line.find_first_of("#");
	len = line.size() - pos;

	if (pos != std::string::npos)
		line.erase(pos, len);
}

template<typename T>
void displayVec(std::vector<T>* v)
{
	typename std::vector<T>::iterator it = v->begin();
	std::cout << "VECTOR: ";
	while (it != v->end())
	{
		std::cout << *it << ' ';
		++it;
	}
	std::cout << std::endl;
}

int ConfParser::parseLine(std::string& line)
{
	size_t	line_nb = 1;
	int		ret = 0;

	// typedef	int		(ConfParser::*t_parse)(std::string&);
	// static t_parse	func[NB_BLOCKS] = {&ConfParser::parseHttp, &ConfParser::parseServer, &ConfParser::parseLocation};

	std::cout << "NATIVE LINE  : " << line << std::endl;
	erase_comments(line);
	std::cout << "LINE w/o com.: " << line << std::endl << std::endl;

	std::istringstream iss(line); 
	for(std::string token; iss >> token && ret == 0; )
	{
		std::cout << "TOKEN " << line_nb << ": " << token << std::endl;
		this->_dir_line->push_back(token);
		if (this->_dir_line->back().find(";") != std::string::npos
			|| this->_dir_line->back().find("}") != std::string::npos
			|| this->_dir_line->back().find("{") != std::string::npos)
		{
			displayVec(this->_dir_line);
			// ret = (this->*func[this->_block_type])(token);
			this->_dir_line->clear();

		}
		if (ret == 1)
			break ;

		line_nb++;
	}
	std::cout << "===================" << std::endl;
	return ret;
}

int ConfParser::parseHttp(std::string& token)
{
	dirMap::iterator it;
	std::cout << "==> HTTP BLOCK" << std::endl << std::endl;
	
	if (token == "http")
		return 0;
	if (token == "server")
	{
		this->_block_type = SERVER;
		return 0;
	}
	if (token == "{")
	{
		if (this->_in_block[HTTP] == FALSE)
		{
			std::cout << "FOUND BRACKET" << std::endl;
			this->_in_block[HTTP] = TRUE;
			return 0;			
		}
		else
			throw UnexpectedTocken("{", this->_line_nb);
	}
	if (this->_in_block[HTTP] == FALSE)
		throw NoOpeningBracket("http", this->_line_nb);
	if (this->_curr_dir == "")
	{
		if ((it = ConfParser::http_map.find(token)) != http_map.end() && this->_in_block[HTTP] == TRUE)
		{
			std::cout << "STORE DIRECTIVE" << std::endl;
			this->_curr_dir = token;
			return 0;
		}
		else if (token != "}")
			throw UnknownDirective(token, this->_line_nb);
	}
	if (this->_curr_dir != "")
	{
		std::cout << "STORE " << this->_curr_dir << " VALUE: " << token << std::endl;
		if (token.find(";") != std::string::npos)
		{	
			this->_curr_dir.clear();
		}
		return 0;
	}
	if (token == "}")
		this->_in_block[HTTP] = FALSE;
	return 0;
}

int ConfParser::parseServer(std::string& token)
{
	if (token == "{")
		this->_nbr_of_srv++;
	if (this->_nbr_of_srv != 0)
		std::cout << "-> SERVER No " << this->_nbr_of_srv << std::endl << std::endl;

	std::cout << "==> SERVER BLOCK" << std::endl << std::endl;
	if (token == "}")
		this->_block_type = HTTP;

	if (token == "location")
		this->_block_type = LOCATION;

	return 0;
}

int ConfParser::parseLocation(std::string& token)
{
	if (token == "{")
		this->_nbr_of_loc++;
	if (this->_nbr_of_loc != 0)
		std::cout << "-> LOCATION No " << this->_nbr_of_loc << std::endl << std::endl;
	std::cout << "==> LOCATION BLOCK" << std::endl << std::endl;
	if (token == "}")
		this->_block_type = SERVER;

	return 0;
}




/*Exceptions*/

ConfParser::UnexpectedTocken::UnexpectedTocken(const std::string token, const size_t line_nb)
: _msg("webserv: unexpected \"" + token + "\":")
{
	std::ostringstream tmp;
	tmp << line_nb;
	this->_msg += tmp.str();

	return;
}

const char* ConfParser::UnexpectedTocken::what() const throw()
{
	return (this->_msg.c_str());
}


ConfParser::NoOpeningBracket::NoOpeningBracket(const std::string token, const size_t line_nb)
: _msg("webserv: directive \"" + token + "\" has no opening \"{\":")
{
	std::ostringstream tmp;
	tmp << line_nb;
	this->_msg += tmp.str();

	return;
}

const char* ConfParser::NoOpeningBracket::what() const throw()
{
	return (this->_msg.c_str());
}

ConfParser::NoClosingBracket::NoClosingBracket(const std::string token, const size_t line_nb)
: _msg("webserv: directive \"" + token + "\" has no closing \"}\":")
{
	std::ostringstream tmp;
	tmp << line_nb;
	this->_msg += tmp.str();

	return;
}

const char* ConfParser::NoClosingBracket::what() const throw()
{
	return (this->_msg.c_str());
}

ConfParser::UnknownDirective::UnknownDirective(const std::string token, const size_t line_nb)
: _msg("webserv: Unknown directive \"" + token + "\":")
{
	std::ostringstream tmp;
	tmp << line_nb;
	this->_msg += tmp.str();

	return;
}

const char* ConfParser::UnknownDirective::what() const throw()
{
	return (this->_msg.c_str());
}