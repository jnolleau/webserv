
// Class that doesnt use GNL to read(): use of an internal buffer to store successive reads (in case request doesnt arrive line by line)
// : use of an internal buffer to store u (in case request doesnt arrive line by linecould rece)TO DO: read_chunked


#ifndef REQUEST_CLASS_H
# define REQUEST_CLASS_H

#include <iostream>
#include <string>
#include <list>
#include <unistd.h> //read
#include "../Utils/get_next_line.h"
#include "../Utils/utils.hpp"

#define BUF_SIZE 1024
#define SUCCESS 1


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

        // attributes
        int fd;
        int error_code;
        std::string error_message;
        request_line req_line;
        std::list<header> headers;
        unsigned long body_size;
        std::string body;
        bool chunked_encoding;
        bool req_line_read;
        std::string buffer;
        
        // processing
        void check_body_headers();
        bool has_transfer_encoding();
        bool has_content_length();
        void store_body_length();
        void store_encoding();
        void store_req_line(std::string line);
        void store_header(std::string line);

        // main

        void parse_buffer();
        void parse_req_line();
        void parse_headers();
        void parse_body();
        void parse_body_normal();
        void parse_body_chunked();
        bool read_buf_line(std::string &line);
        
        void read_from_socket();

    
    public:
        // constructor & destructor
        Request(void);
        Request(int fd);
        Request(const Request &copy);
        virtual ~Request(void);
        Request&  operator=(const Request &copy);

        bool end_of_connection;
        bool request_ready;
        // main functions
        void parse();
        void reset();

        // utils
        void print();

        // getters & setters
        int get_error_code() const;
        void set_error_code(int code);
};

bool content_length_present(std::pair<std::string, std::string> header);
bool transfer_encoding_present(std::pair<std::string, std::string> header);

#endif