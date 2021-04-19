/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/16 15:55:10 by julnolle          #+#    #+#             */
/*   Updated: 2021/04/19 19:27:18 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfParser.hpp"

ConfParser::ConfParser(void)
: _configFile(DEFAULT_CONF_FILE), _httpBlock(),
_block_type(HTTP), _line_nb(1), _nbr_of_srv(0),
_nbr_of_loc(0), _in_block(FALSE)
{
	return;
}

ConfParser::ConfParser(const std::string filename)
: _configFile(filename), _httpBlock(),
_block_type(HTTP), _line_nb(1), _nbr_of_srv(0),
_nbr_of_loc(0), _in_block(FALSE)
{
	return; 
}

ConfParser::~ConfParser(void)
{
	std::cout << "DESTRUCTOR CALLED" << std::endl;
	return ;
}

/**
 * Static attributs
 *
 * Contains directive listing assiociated
 * with their function handler 
 */

typedef std::map<std::string, int (ConfParser::*)(void)>	dirMap; //move this typedef in higher level ?

dirMap	ConfParser::http_map = setHttpMap();
dirMap	ConfParser::srv_map = setSrvMap();
dirMap	ConfParser::loc_map = setLocMap();

dirMap	ConfParser::setHttpMap()
{
	dirMap map;

	map["root"] = &ConfParser::setRoot;
	map["autoindex"] = &ConfParser::setAutoIndex;
	map["index"] = &ConfParser::setIndex;
	map["limit_except"] = &ConfParser::setAllowedMethods;
	map["error_page"] = &ConfParser::setErrorPage;
	map["client_max_body_size"] = &ConfParser::setMaxBdySize;
	map["keepalive_timeout"] = &ConfParser::setKeepAlive;
	map["chunked_enc"] = &ConfParser::setChunkEnc;
	map["auth_basic"] = &ConfParser::setAuthBasic;
	map["auth_basic_user_file"] = &ConfParser::setAuthBasicFile;
	map["cgi_param"] = &ConfParser::setCgiParam;
	map["cgi_pass"] = &ConfParser::setCgiPass;
	map["include"] = &ConfParser::parseInclude;

	return map;
}

dirMap	ConfParser::setSrvMap()
{
	dirMap map;

	map["listen"] = &ConfParser::setListen;
	map["root"] = &ConfParser::setRoot;
	map["server_name"] = &ConfParser::setServerName;
	map["autoindex"] = &ConfParser::setAutoIndex;
	map["index"] = &ConfParser::setIndex;
	map["limit_except"] = &ConfParser::setAllowedMethods;
	map["error_page"] = &ConfParser::setErrorPage;
	map["client_max_body_size"] = &ConfParser::setMaxBdySize;
	map["keepalive_timeout"] = &ConfParser::setKeepAlive;
	map["chunked_enc"] = &ConfParser::setChunkEnc;
	map["auth_basic"] = &ConfParser::setAuthBasic;
	map["auth_basic_user_file"] = &ConfParser::setAuthBasicFile;
	map["cgi_param"] = &ConfParser::setCgiParam;
	map["cgi_pass"] = &ConfParser::setCgiPass;

	return map;
}

dirMap	ConfParser::setLocMap()
{
	dirMap map;

	map["root"] = &ConfParser::setRoot;
	map["autoindex"] = &ConfParser::setAutoIndex;
	map["index"] = &ConfParser::setIndex;
	map["limit_except"] = &ConfParser::setAllowedMethods;
	map["error_page"] = &ConfParser::setErrorPage;
	map["client_max_body_size"] = &ConfParser::setMaxBdySize;
	map["keepalive_timeout"] = &ConfParser::setKeepAlive;
	map["chunked_enc"] = &ConfParser::setChunkEnc;
	map["auth_basic"] = &ConfParser::setAuthBasic;
	map["auth_basic_user_file"] = &ConfParser::setAuthBasicFile;
	map["cgi_param"] = &ConfParser::setCgiParam;
	map["cgi_pass"] = &ConfParser::setCgiPass;

	return map;
}

int ConfParser::setListen(void)
{
	this->checkNbrOfArgs(2, &same_as<size_t>);

	size_t pos = this->_dir_line[1].find(":");

	if (pos != std::string::npos)
	{
		if (this->_httpBlock.getLastServer().setListenIp(this->_dir_line[1].substr(0, pos)) == FAILURE)
			throw UnknownHost(this->_dir_line[1], this);
		if (this->_httpBlock.getLastServer().setListenPort(this->_dir_line[1].substr(pos + 1).c_str()) == FAILURE)
			throw InvalidPort(this->_dir_line[1], this);
	}
	else if  (pos == std::string::npos && ft_isdigit_string(this->_dir_line[1])) 
	{
		if (this->_httpBlock.getLastServer().setListenPort(this->_dir_line[1].c_str()) == FAILURE)
			throw InvalidPort(this->_dir_line[1], this);
	}
	else if (pos == std::string::npos)
	{
		if (this->_httpBlock.getLastServer().setListenIp(this->_dir_line[1]) == FAILURE)
			throw UnknownHost(this->_dir_line[1], this);
	}
	else
		throw InvalidValue(this->_dir_line[0], this);
	
	// std::cout << "SET LISTEN FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setServerName(void)
{
	this->checkNbrOfArgs(1, &is_higher<size_t>);

	std::vector<std::string>::iterator first = this->_dir_line.begin();

	this->_httpBlock.getLastServer().setServerNames(++first, this->_dir_line.end());

	// std::cout << "SET SRVNAME FUNCTION" << std::endl;
	return 0;
}

int ConfParser::setRoot(void)
{
	this->checkNbrOfArgs(2, &same_as<size_t>);

	if (this->_block_type == HTTP)
	{
		this->_httpBlock.setRoot(this->_dir_line[1]);
	}
	else if (this->_block_type == SERVER)
	{
		this->_httpBlock.getLastServer().setRoot(this->_dir_line[1]);
	}
	if (this->_block_type == LOCATION)
	{
		this->_curr_location->setRoot(this->_dir_line[1]);
	}

	// std::cout << "SET ROOT FUNCTION" << std::endl;
	return 0;
}

int ConfParser::setAutoIndex(void)
{
	this->checkNbrOfArgs(2, &same_as<size_t>);

	changeCaseString(this->_dir_line[1], ::tolower);

	if (this->_dir_line[1] != "on" && this->_dir_line[1] != "off")
		throw InvalidValue(this->_dir_line[0], this);

	if (this->_block_type == HTTP)
		this->_httpBlock.setAutoIndex(this->_dir_line[1]);
	else if (this->_block_type == SERVER)
		this->_httpBlock.getLastServer().setAutoIndex(this->_dir_line[1]);
	if (this->_block_type == LOCATION)
		this->_curr_location->setAutoIndex(this->_dir_line[1]);
	// std::cout << "SET AUTOIDX FUNCTION" << std::endl;
	return 0;
}

int ConfParser::setIndex(void)
{
	this->checkNbrOfArgs(1, &is_higher<size_t>);

	std::vector<std::string>::iterator first = this->_dir_line.begin();
	if (this->_block_type == HTTP)
		this->_httpBlock.setIndexes(++first, this->_dir_line.end());
	else if (this->_block_type == SERVER)
		this->_httpBlock.getLastServer().setIndexes(++first, this->_dir_line.end());
	if (this->_block_type == LOCATION)
		this->_curr_location->setIndexes(++first, this->_dir_line.end());

	// std::cout << "SET INDX FUNCTION" << std::endl;
	return 0;
}

int ConfParser::setAllowedMethods(void)
{
	this->checkNbrOfArgs(1, &is_higher<size_t>);

	std::vector<std::string>::iterator first = this->_dir_line.begin();
	if (this->_block_type == HTTP)
		this->_httpBlock.setLimitExcept(++first, this->_dir_line.end());
	else if (this->_block_type == SERVER)
		this->_httpBlock.getLastServer().setLimitExcept(++first, this->_dir_line.end());
	if (this->_block_type == LOCATION)
		this->_curr_location->setLimitExcept(++first, this->_dir_line.end());
	// std::cout << "SET ALLMETH FUNCTION" << std::endl;
	return 0;
}

int ConfParser::setErrorPage(void)
{
	int ret = SUCCESS;
	this->checkNbrOfArgs(2, &is_higher<size_t>);

	std::vector<std::string>::iterator first = this->_dir_line.begin();
	std::vector<std::string>::iterator end = this->_dir_line.end();

	for (size_t i = 1; i < this->_dir_line.size() - 1; ++i)
	{
		if (!ft_isdigit_string(this->_dir_line[i]))
			throw InvalidValue(this->_dir_line[0], this);
	}

	if (this->_block_type == HTTP)
		ret  = this->_httpBlock.setErrorPages(++first, --end, this->_dir_line.back());
	else if (this->_block_type == SERVER)
	{
		this->_httpBlock.getLastServer().setErrorPages(++first, --end, this->_dir_line.back());
	}
	if (this->_block_type == LOCATION)
	{
		this->_curr_location->setErrorPages(++first, --end, this->_dir_line.back());
	}
	if (ret == FAILURE)
		throw InvalidValue(this->_dir_line[0], this);

	// std::cout << "SET ERROPAGE FUNCTION" << std::endl;
	return 0;
}

int ConfParser::setMaxBdySize(void)
{
	this->checkNbrOfArgs(2, &same_as<size_t>);

	if (!ft_isdigit_string(this->_dir_line[1]))
		throw InvalidValue(this->_dir_line[0], this);

	int mbs = atoi(this->_dir_line[1].c_str());

	if (this->_block_type == HTTP)
	{
		this->_httpBlock.setMaxBdySize(mbs);
		// this->_httpBlock.getLastServer().setMaxBdySize(mbs);
	}
	else if (this->_block_type == SERVER)
	{
		this->_httpBlock.getLastServer().setMaxBdySize(mbs);
	}
	if (this->_block_type == LOCATION)
	{
		this->_curr_location->setMaxBdySize(mbs);
	}
	// std::cout << "SET MAXBDYSIZE FUNCTION" << std::endl;
	return 0;
}

int ConfParser::setKeepAlive(void)
{
	this->checkNbrOfArgs(2, &same_as<size_t>);

	if (!ft_isdigit_string(this->_dir_line[1]))
		throw InvalidValue(this->_dir_line[0], this);

	int mbs = atoi(this->_dir_line[1].c_str());

	if (this->_block_type == HTTP)
		this->_httpBlock.setKeepaliveTimeout(mbs);
	else if (this->_block_type == SERVER)
	{
		this->_httpBlock.getLastServer().setKeepaliveTimeout(mbs);
	}
	if (this->_block_type == LOCATION)
	{
		this->_curr_location->setKeepaliveTimeout(mbs);
	}

	// std::cout << "SET keepalive FUNCTION" << std::endl;
	return 0;
}

int ConfParser::setChunkEnc(void)
{
	this->checkNbrOfArgs(2, &same_as<size_t>);

	changeCaseString(this->_dir_line[1], ::tolower);

	if (this->_dir_line[1] != "on" && this->_dir_line[1] != "off")
		throw InvalidValue(this->_dir_line[0], this);

	if (this->_block_type == HTTP)
		this->_httpBlock.setChunkedEncoding(this->_dir_line[1]);
	else if (this->_block_type == SERVER)
		this->_httpBlock.getLastServer().setChunkedEncoding(this->_dir_line[1]);
	if (this->_block_type == LOCATION)
		this->_curr_location->setChunkedEncoding(this->_dir_line[1]);

	// std::cout << "SET CHUNKED ENC" << std::endl;
	return 0;
}

int ConfParser::setAuthBasic(void)
{
	this->checkNbrOfArgs(1, &is_higher<size_t>);
	
	std::string area_name;
	
	if (this->_dir_line.size() == 2)
		area_name = changeCaseString(this->_dir_line[1], ::tolower);
	else
	{
		size_t size = this->_dir_line.size();
		for (size_t i = 1; i < size - 2; ++i)
		{
			area_name += this->_dir_line[i] += ' ';
		}
		area_name += this->_dir_line[size - 1];

		// pos = area_name.find_first_of("\"");
		// if (pos != std::string::npos)
		// 	area_name.erase(pos, 1);
		// pos = area_name.find_last_of("\"");
		// if (pos != std::string::npos)
		// 	area_name.erase(pos, 1);
	}
	
	if (this->_block_type == HTTP)
		this->_httpBlock.setAuthBasic(area_name);
	else if (this->_block_type == SERVER)
		this->_httpBlock.getLastServer().setAuthBasic(area_name);
	if (this->_block_type == LOCATION)
		this->_curr_location->setAuthBasic(area_name);

	// std::cout << "SET AUTH_BASIC" << std::endl;
	return 0;
}

int ConfParser::setAuthBasicFile(void)
{
	this->checkNbrOfArgs(2, &same_as<size_t>);

	if (this->_block_type == HTTP)
		this->_httpBlock.setAuthBasicFile(this->_dir_line[1]);
	else if (this->_block_type == SERVER)
		this->_httpBlock.getLastServer().setAuthBasicFile(this->_dir_line[1]);
	if (this->_block_type == LOCATION)
		this->_curr_location->setAuthBasicFile(this->_dir_line[1]);

	// std::cout << "SET AUTH_BASIC_FILE" << std::endl;
	return 0;
}

int ConfParser::setCgiParam(void)
{
	// std::cout << "SET CGIPARAM FUNCTION" << std::endl;
	return 0;
}
int ConfParser::setCgiPass(void)
{
	// std::cout << "SET CGIPASS FUNCTION" << std::endl;
	return 0;
}

int ConfParser::parseInclude()
{

	displayVec(this->_dir_line);
	this->checkNbrOfArgs(2, &same_as<size_t>);
	
	this->readConfFile(this->_dir_line[1]);
	// std::cout << "PARSE INCLUDE FUNCTION" << std::endl;
	return 0;
}

template <class Compare>
void ConfParser::checkNbrOfArgs(size_t expected_nbr, Compare comp)
{
	if (!comp(expected_nbr, this->_dir_line.size()))
		throw InvalidNbrOfArgs(this->_dir_line[0], this);
}

void ConfParser::readConfFile(const std::string& confFile)
{
	std::ifstream	file;
	std::string		line;
	size_t			line_nb_save = this->_line_nb;

	this->_configFile = confFile;	
	file.open(confFile.c_str());
	if (file.fail())
		throw FileOperationFail("open", this);

	this->_dir_line.clear();
	this->_line_nb = 1;
	while (getline(file, line))
	{
		this->parseLine(line);

		file.clear();
		this->_line_nb++;
	}
	file.clear();

	file.close();
	if (file.fail())
		throw FileOperationFail("close", this);
	
	if (this->_in_block.test(SERVER)
		|| this->_in_block.test(LOCATION)
		|| (this->_in_block.test(HTTP) && confFile == DEFAULT_CONF_FILE)
	)
		throw UnexpectedEOF("end of file", this);

	this->_line_nb = line_nb_save;
	this->_configFile = DEFAULT_CONF_FILE;
	return ;
}

void ConfParser::erase_comments(std::string& line)
{
	size_t pos;
	size_t len = 0;

	pos = line.find_first_of("#");
	len = line.size() - pos;

	if (pos != std::string::npos)
		line.erase(pos, len);
}

void ConfParser::parseLine(std::string& line)
{
	// delete all text after # in a line
	erase_comments(line);

	std::istringstream iss(line); 
	for(std::string token; iss >> token;)
	{
		this->_dir_line.push_back(token);
		if (this->_dir_line.back().find_first_of("{;}") != std::string::npos)
		{
			this->parseDirective();
			this->_dir_line.clear();
		}
	}
	// if (line != "")
	// 	std::cout << "===================" << std::endl;
}

void ConfParser::handleBlockIn(const std::string& directive)
{
	if (directive == "http")
	{
		if (this->_in_block[HTTP] == TRUE)
			throw UnexpectedTocken("http", this);
		if (this->_dir_line.at(1) == "{")
			this->_in_block[HTTP] = TRUE;
		else
			throw NoOpeningBracket("http", this);
		return ;
	}
	if (directive == "server")
	{
		if (this->_in_block[SERVER] == TRUE)
			throw UnexpectedTocken("server", this);
		this->_block_type = SERVER;
		if (this->_dir_line.at(1) == "{")
		{
			if (this->_in_block[SERVER] == FALSE)
				this->_in_block[SERVER] = TRUE;
			else
				throw NoClosingBracket("server", this);

			this->_httpBlock.addServer();
		}
		else
			throw NoOpeningBracket("server", this);
		return ;
	}
	if (directive == "location")
	{
		this->checkNbrOfArgs(3, &same_as<size_t>);
		this->_block_type = LOCATION;
		if (this->_dir_line.at(2) == "{")
		{
			if (this->_in_block[LOCATION] == FALSE)
				this->_in_block[LOCATION] = TRUE;
			else
				throw NoClosingBracket("location", this);

			std::pair<std::map<std::string, LocationBlock>::iterator, bool> newLoc;
			newLoc = this->_httpBlock.getLastServer().addLocation(this->_dir_line[1]);
			if (newLoc.second)
			{
				this->_curr_location = &newLoc.first->second;
				this->_curr_location->setPath(this->_dir_line[1]); //Optional since 
																// locations are stored in map
			}
			else
				throw DuplicateLocation(this->_dir_line[1], this);
		}
		else
			throw NoOpeningBracket("location", this);
	}
}

void ConfParser::handleBlockOut()
{
	if (this->_block_type == HTTP)
	{
		this->_in_block[HTTP] = FALSE;
		// std::cout << "close http block" << std::endl;
	}
	else if (this->_block_type == SERVER)
	{
		this->_in_block[SERVER] = FALSE;
		this->_block_type = HTTP;
		// std::cout << "close server block" << std::endl;
	}
	else if (this->_block_type == LOCATION)
	{
		this->_in_block[LOCATION] = FALSE;
		this->_block_type = SERVER;
		// std::cout << "close location block" << std::endl;
	}
}

void ConfParser::parseDirective()
{
	dirMap::iterator it;
	std::string directive = this->_dir_line.at(0);
	
/*	if (this->_block_type == HTTP)
		std::cout << "==> HTTP BLOCK" << std::endl << std::endl;
	else if (this->_block_type == SERVER)
		std::cout << "==> SERVER BLOCK" << std::endl << std::endl;
	if (this->_block_type == LOCATION)
		std::cout << "==> LOCATION BLOCK" << std::endl << std::endl;
*/
	size_t pos = this->_dir_line.back().find_last_of(";");
	if (pos != std::string::npos)
	{
		this->_dir_line.back().erase(pos, 1);
		if (this->_dir_line.back().empty())
			this->_dir_line.pop_back();
	}
	
	// displayVec(this->_dir_line);

	if (directive == "http" || directive == "server" || directive == "location")
	{
		this->handleBlockIn(directive);
		return ;
	}

	if (this->_dir_line.at(0) == "{")
		throw UnexpectedTocken("{", this);

	if (this->_dir_line.at(0) == "}")
	{
		this->handleBlockOut();
		return ;
	}


	
	if (this->_block_type == HTTP)
	{
		if (ConfParser::http_map.find(directive) != http_map.end())
			(this->*ConfParser::http_map[directive])();
		else
			throw UnknownDirective(directive, this);
	}
	else if (this->_block_type == SERVER)
	{
		if (ConfParser::srv_map.find(directive) != srv_map.end())
			(this->*ConfParser::srv_map[directive])();
		else
			throw UnknownDirective(directive, this);
	}
	else if (this->_block_type == LOCATION)
	{
		if (ConfParser::loc_map.find(directive) != loc_map.end())
			(this->*ConfParser::loc_map[directive])();
		else
			throw UnknownDirective(directive, this);
	}
}

/*Getters*/

HttpBlock&		ConfParser::getHttpBlock(void)
{
	return this->_httpBlock;
}


/*Exceptions*/

ConfParser::UnexpectedTocken::UnexpectedTocken(const std::string token, ConfParser *p)
: _msg("webserv: unexpected \"" + token + "\" in " + p->_configFile + ":")
{
	std::ostringstream tmp;
	tmp << p->_line_nb;
	this->_msg += tmp.str();

	return;
}

const char* ConfParser::UnexpectedTocken::what() const throw()
{
	return (this->_msg.c_str());
}


ConfParser::NoOpeningBracket::NoOpeningBracket(const std::string token, ConfParser *p)
: _msg("webserv: directive \"" + token + "\" has no opening \"{\" in " + p->_configFile + ":")
{
	std::ostringstream tmp;
	tmp << p->_line_nb;
	this->_msg += tmp.str();

	return;
}

const char* ConfParser::NoOpeningBracket::what() const throw()
{
	return (this->_msg.c_str());
}

ConfParser::NoClosingBracket::NoClosingBracket(const std::string token, ConfParser *p)
: _msg("webserv: directive \"" + token + "\" has no closing \"}\" in " + p->_configFile + ":")
{
	std::ostringstream tmp;
	tmp << p->_line_nb;
	this->_msg += tmp.str();

	return;
}

const char* ConfParser::NoClosingBracket::what() const throw()
{
	return (this->_msg.c_str());
}

ConfParser::UnexpectedEOF::UnexpectedEOF(const std::string token, ConfParser *p)
: _msg("webserv: unexpected " + token + ", expecting \"}\" in " + p->_configFile + ":")
{
	std::ostringstream tmp;
	tmp << p->_line_nb;
	this->_msg += tmp.str();

	return;
}

const char* ConfParser::UnexpectedEOF::what() const throw()
{
	return (this->_msg.c_str());
}

ConfParser::UnknownDirective::UnknownDirective(const std::string token, ConfParser *p)
: _msg("webserv: Unknown directive \"" + token + "\" in " + p->_configFile + ":")
{
	std::ostringstream tmp;
	tmp << p->_line_nb;
	this->_msg += tmp.str();

	return;
}

const char* ConfParser::UnknownDirective::what() const throw()
{
	return (this->_msg.c_str());
}

ConfParser::InvalidNbrOfArgs::InvalidNbrOfArgs(const std::string token, ConfParser *p)
: _msg("webserv: invalid number of arguments in \"" + token + "\" directive in " + p->_configFile + ":")
{
	std::ostringstream tmp;
	tmp << p->_line_nb;
	this->_msg += tmp.str();

	return;
}

const char* ConfParser::InvalidNbrOfArgs::what() const throw()
{
	return (this->_msg.c_str());
}

ConfParser::InvalidValue::InvalidValue(const std::string token, ConfParser *p)
: _msg("webserv: \"" + token + "\" directive invalid value in " + p->_configFile + ":")
{
	std::ostringstream tmp;
	tmp << p->_line_nb;
	this->_msg += tmp.str();

	return;
}

const char* ConfParser::InvalidValue::what() const throw()
{
	return (this->_msg.c_str());
}

ConfParser::FileOperationFail::FileOperationFail(const std::string op, ConfParser *p)
: _msg("webserv: cannot " + op + " file \"" + p->_configFile + "\"")
{
	return;
}

const char* ConfParser::FileOperationFail::what() const throw()
{
	return (this->_msg.c_str());
}

ConfParser::UnknownHost::UnknownHost(const std::string token, ConfParser *p)
: _msg("webserv: host not found in \"" + token + "\" of the \"listen\" directive in " + p->_configFile + ":")
{
	std::ostringstream tmp;
	tmp << p->_line_nb;
	this->_msg += tmp.str();

	return;
}

const char* ConfParser::UnknownHost::what() const throw()
{
	return (this->_msg.c_str());
}

ConfParser::InvalidPort::InvalidPort(const std::string token, ConfParser *p)
: _msg("webserv: invalid port in \"" + token + "\" of the \"listen\" directive in " + p->_configFile + ":")
{
	std::ostringstream tmp;
	tmp << p->_line_nb;
	this->_msg += tmp.str();

	return;
}

const char* ConfParser::InvalidPort::what() const throw()
{
	return (this->_msg.c_str());
}

ConfParser::DuplicateLocation::DuplicateLocation(const std::string token, ConfParser *p)
: _msg("webserv: duplicate location \"" + token + "\" in " + p->_configFile + ":")
{
	std::ostringstream tmp;
	tmp << p->_line_nb;
	this->_msg += tmp.str();

	return;
}

const char* ConfParser::DuplicateLocation::what() const throw()
{
	return (this->_msg.c_str());
}
