#pragma once
#include "TCPServer.h"
#include <unordered_map>
#include "pystring.h"
#include <fstream>
#include <sstream>

enum HTTPRequestMethod {
	GET,
	POST,
	PUT,
	OPTIONS,
};


typedef struct {
	HTTPRequestMethod requestmethod;
	std::string route;
	std::unordered_map<std::string, std::string> headers;
	std::string body;
}HTTPRequest, *PHTTPRequest;




class WebServer: TCPServer {

	HTTPRequest request;
	std::unordered_map<std::string,std::string> routes;

public:
	
	WebServer();

	void handle_client(ClientInfo client);
	
	void parserequest(std::string& data);

	std::string& get_route();

	std::unordered_map<std::string, std::string>& get_headers(); 

	std::string& get_body();

	void add_route(const char* route, const char* htmlfile);


	std::string get_file_contents(std::string& filename);


	void start(std::string ip, unsigned short port);

};
