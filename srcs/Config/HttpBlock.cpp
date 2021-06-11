/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HttpBlock.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: julnolle <julnolle@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/03/18 09:29:05 by julnolle          #+#    #+#             */
/*   Updated: 2021/06/11 16:06:05 by julnolle         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HttpBlock.hpp"

HttpBlock::HttpBlock(void) :
_autoindex(NOT_SET),
_client_max_body_size(NOT_SET),
_keepalive_timeout(NOT_SET),
_chunked_transfer_encoding(NOT_SET),
_auth_basic(DEFAULT_AUTH_BASIC) {
	// std::cout << GREEN << "HTTP CTOR" << NOCOLOR << std::endl;
	return ;
}

HttpBlock::HttpBlock(HttpBlock const & copy)
{
	// std::cout << YELLOW << "HTTP CPY CTOR" << NOCOLOR << std::endl;
	this->_root = copy._root;
	this->_autoindex = copy._autoindex;
	this->_indexes = copy._indexes;
	this->_error_pages = copy._error_pages;
	this->_client_max_body_size = copy._client_max_body_size;
	this->_keepalive_timeout = copy._keepalive_timeout;
	this->_chunked_transfer_encoding = copy._chunked_transfer_encoding;
	this->_auth_basic = copy._auth_basic;
	this->_auth_basic_user_file = copy._auth_basic_user_file;
	this->_cgi_allowed_ext = copy._cgi_allowed_ext;
	this->_returns = copy._returns;

	return ;
}

HttpBlock::~HttpBlock(void)
{
	return ;
}

HttpBlock& HttpBlock::operator=(HttpBlock const & rhs)
{
	this->_root = rhs._root;
	this->_autoindex = rhs._autoindex;
	this->_indexes = rhs._indexes;
	this->_error_pages = rhs._error_pages;
	this->_client_max_body_size = rhs._client_max_body_size;
	this->_keepalive_timeout = rhs._keepalive_timeout;
	this->_chunked_transfer_encoding = rhs._chunked_transfer_encoding;
	this->_auth_basic = rhs._auth_basic;
	this->_auth_basic_user_file = rhs._auth_basic_user_file;
	this->_cgi_allowed_ext = rhs._cgi_allowed_ext;
	this->_returns = rhs._returns;

	return *this;
}


void	HttpBlock::setRoot(std::string const& path)
{
	this->_root = path;
}

void	HttpBlock::setAutoIndex(std::string const& state)
{
	if (state == "on")
		this->_autoindex = ON;
	else if (state == "off")
		this->_autoindex = OFF;
}

void	HttpBlock::setIndexes(strVecIterator first, strVecIterator last)
{
	this->_indexes.assign(first, last);
}

void	HttpBlock::setLimitExcept(strVecIterator first, strVecIterator last)
{
	this->_limit_except.assign(first, last);
}

int		HttpBlock::setErrorPages(strVecIterator first, strVecIterator last, std::string const& val)
{
	int key(0);

	while (first != last)
	{
		key = atoi(first->c_str());
		if (key < 300 || key > 599)
			return (FAILURE);
		this->_error_pages.insert(std::make_pair(key, val));
		++first;
	}
	return (SUCCESS);
}

void HttpBlock::setMaxBdySize(size_type const& size)
{
	this->_client_max_body_size = size;
}
void HttpBlock::setKeepaliveTimeout(size_type const& timeout)
{
	this->_keepalive_timeout = timeout;
}

void	HttpBlock::setChunkedEncoding(std::string const& state)
{
	if (state == "on")
		this->_chunked_transfer_encoding = ON;
	if (state == "off")
		this->_chunked_transfer_encoding = OFF;
}

void	HttpBlock::setAuthBasic(std::string const& value)
{
	this->_auth_basic = value;
}

void	HttpBlock::setAuthBasicFile(std::string const& path)
{
	this->_auth_basic_user_file = path;
}

void	HttpBlock::setCgiAllowedExt(strVecIterator first, strVecIterator last)
{
	this->_cgi_allowed_ext.assign(first, last);
}

int		HttpBlock::setReturn(std::string const& code, std::string const& url)
{
	int key(0);

		key = atoi(code.c_str());
		if (key < 300 || key > 599)
			return (FAILURE);
		this->_returns.insert(std::make_pair(key, url));

	return (SUCCESS);
}

// Getters
const std::string&	HttpBlock::getRoot(void) const
{
	return this->_root;
}

const int&			HttpBlock::getAutoindex(void) const
{
	return this->_autoindex;
}

const stringVec&	HttpBlock::getIndexes(void) const
{
	return this->_indexes;
}

const stringVec&	HttpBlock::getLimitExcept(void) const
{
	return this->_limit_except;
}

const errorMap&		HttpBlock::getErrorPages(void) const
{
	return this->_error_pages;
}

const long int&		HttpBlock::getMaxBdySize(void) const
{
	return this->_client_max_body_size;
}

const long int&		HttpBlock::getKeepaliveTimeout(void) const
{
	return this->_keepalive_timeout;
}

const int&			HttpBlock::getChunkedEncoding(void) const
{
	return this->_chunked_transfer_encoding;
}

const std::string&	HttpBlock::getAuthBasic(void) const
{
	return this->_auth_basic;
}

const std::string&	HttpBlock::getAuthBasicFile(void) const
{
	return this->_auth_basic_user_file;
}

const stringVec&	HttpBlock::getCgiAllowedExt(void) const
{
	return this->_cgi_allowed_ext;
}

const redirMap&	HttpBlock::getReturns(void) const
{
	return this->_returns;
}

std::ostream & operator<<(std::ostream & o, HttpBlock const & rhs)
{

	o << "HTTP BLOCK: " << std::endl;
	o << "------------" << std::endl;

	o << "ROOT: " << rhs.getRoot() << std::endl;
	o << "AUTOINDEX: "; putState(o, rhs.getAutoindex());

	o << "INDEXES: ";
	putVecToOstream(o, rhs.getIndexes().begin(), rhs.getIndexes().end());
	
	o << "LIMIT_EXCEPT: ";
	putVecToOstream(o, rhs.getLimitExcept().begin(), rhs.getLimitExcept().end());

	o << "ERROR PAGES: ";
	putMapToOstream(o, rhs.getErrorPages().begin(), rhs.getErrorPages().end());
	
	o << "MAX BDY SIZE: " << rhs.getMaxBdySize() << std::endl;
	o << "KEEP. TIMEOUT: " << rhs.getKeepaliveTimeout() << std::endl;

	o << "CHUNKED ENC.: "; putState(o, rhs.getChunkedEncoding());
	
	o << "AUTH BASIC: " << rhs.getAuthBasic() << std::endl;
	o << "AUTH BASIC FILE: " << rhs.getAuthBasicFile() << std::endl;
	o << "CGI ALLOWED EXT: ";
	putVecToOstream(o, rhs.getCgiAllowedExt().begin(), rhs.getCgiAllowedExt().end());

	return o;
}

