
#include <iostream>

#include "TCPServer.h"
#include "WebServer.h"


int main() {


	/*std::string req = "GET /testroute HTTP/1.1\r\n"
		"Content-Type: text/html\r\n"
		"Host: www.google.com\r\n"
		"User-Agent: FBI agent\r\n"
		"Accept: text/html\r\n\r\n"
		"Testbody";
		


	
	std::string a("ab\r\n");
	WebServer ws;
	ws.parserequest(req);
	return 0;*/

	/*std::string ip = "127.0.0.1";
	std::expected<std::unique_ptr<TCPServer>, int> result = InitializeTCPServer(ip, 1234);

	if (result.has_value()) {
		
		std::unique_ptr<TCPServer> tcpserver  = std::move(result.value());
		if (tcpserver->bindto()) {
		
			std::cout << "Server socket successfully bound to: " << 
				tcpserver->get_ipaddress()<< ":" << tcpserver->get_portnumber() << std::endl;
		
			if (tcpserver->startlistening(10)) {
				std::cout << "started listening\n";
				
				while (tcpserver->shalliaccept()) {
					
					if (tcpserver->startaccepting()) {
						ClientInfo client = tcpserver->get_latest_client();
						WebServer ws;
						ws.handle_client(client);


					}

				}


			}

		
		}
		

	}*/



	WebServer ws;
	ws.add_route("/", "index.html");
	ws.add_route("/health", "health.html");

	std::string ip = "127.0.0.1";
	ws.start(ip, 1234);




	return 0;

}
