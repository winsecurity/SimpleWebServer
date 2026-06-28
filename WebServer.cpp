#include "WebServer.h"




void WebServer::handle_client(ClientInfo client) {

	SOCKET clientsocket = client.clientsocket;



	char recvbuffer[40960]{ 0 };
	int res = recv(clientsocket, recvbuffer, sizeof(recvbuffer) / sizeof(recvbuffer[0]), 0);
	if (res == SOCKET_ERROR) {
		std::cout << "receiving from client failed: " << WSAGetLastError() << std::endl;

	}

	std::cout << "[+] Received from client: " << recvbuffer << std::endl;
	std::string recved(recvbuffer);
	this->parserequest(recved);
	auto route = this->get_route();
	std::string filecontents = get_file_contents(routes[route]);



	if (recved == "quit" || recved == "QUIT") {
		std::cout << "client sent us quit, so quitting\n";

	}


	

	std::string response =
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: " + std::to_string(filecontents.length()) + "\r\n"
		"Connection: close\r\n"
		"\r\n" +
		filecontents;

	res = send(clientsocket, &response[0], response.length(), 0);
	if (res == SOCKET_ERROR) {
		std::cout << "sending to client failed: " << WSAGetLastError() << std::endl;

	}

	closesocket(clientsocket);


}




void WebServer::parserequest(std::string& data) {

	std::vector<std::string> content = pystring::split(data, "\r\n");

	
	// reqline
	// ["GET", "/testroute","HTTP/1.1"]
	std::vector<std::string> reqline = pystring::split(content[0], " ");

	

	if (reqline[0] == "GET") {
		this->request.requestmethod = GET;
	}
	request.route = reqline[1];
	std::string httpmethod = reqline[2];


	for (std::string& line : content) {
		
		if (line.contains(":")) {
			std::vector<std::string> headerline = pystring::split(line, ":");

			this->request.headers[headerline[0]] = headerline[1];
		}

	}


	std::cout << "route:  '" << request.route << "'" << std::endl;
	for (auto i = request.headers.begin(); i != request.headers.end();i++){
		std::cout << i->first << ": " << i->second << std::endl;
	}

	request.body = content.back();
	std::cout << "line: " << this->request.body << std::endl;


}


std::string& WebServer::get_route() { return this->request.route; }


std::unordered_map<std::string, std::string>& WebServer::get_headers() { return request.headers; }


std::string& WebServer::get_body() { return request.body; }


void WebServer::add_route(const char* route, const char* htmlfile) {


	this->routes[route] = htmlfile;

}



std::string WebServer::get_file_contents(std::string& filename) {

	std::fstream inputfile;
	inputfile.open(filename);

	if (inputfile.fail()) {
		return std::string("<h1>File not found</h1>");
	}

	std::stringstream buffer;
	buffer << inputfile.rdbuf();

	inputfile.close();
	return buffer.str();

}



void WebServer::start(std::string ip, unsigned short port) {

	std::expected<std::unique_ptr<TCPServer>,int> result =  InitializeTCPServer(ip, port);

	if (result.has_value()) {

		std::unique_ptr<TCPServer> tcpserver = std::move(result.value());

		if (tcpserver->bindto()) {
			std::cout << "Server bound to " << tcpserver->get_ipaddress() << ":" << tcpserver->get_portnumber() << std::endl;
			
			if (tcpserver->startlistening(SOMAXCONN)) {

				while (tcpserver->shalliaccept()) {

					if (tcpserver->startaccepting()) {

						ClientInfo clientinfo = tcpserver->get_latest_client();

						this->handle_client(clientinfo);
						
					}


				}

			}

		
		}

		else {
			std::cout << "unable to bind to the ip and port\n";
			return;
		}

	}
	
	else {
		std::cout << "unable to initialize TCPServer: " << result.error() << std::endl;
	}

}



WebServer::WebServer() {

}
