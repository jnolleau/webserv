#ifndef REQUEST_CLASS_H
# define REQUEST_CLASS_H

#include <iostream>
#include <string>
#include <list>


struct request_line
{
    std::string method;
    std::string target;
    std::string version;
};

class Request
{
    private:
        typedef std::pair<std::string, std::string> header;

        int error_code;
        request_line req_line;
        std::list<header> headers;
        int body_size;
        std::string body;
        
        bool has_transfer_encoding();
        bool has_content_lenght();
        void store_body_lenght();
    
    public:
        Request(void);
        Request(const Request &copy);
        virtual ~Request(void);
        Request&  operator=(const Request &copy);

        void add_req_line(std::string line);
        void add_header(std::string line);
        void add_body(std::string body);
        void print();
        void parse_body_headers();

        // getters
        int get_error_code() const;
        int get_body_lenght() const;
};

bool is_whitespace(char c);
void trim_whitespace(std::string &s);

#endif