#pragma once

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <ws2tcpip.h>
#include <Winsock2.h>
#include <iostream>
#include <string>
#include <expected>
#include <memory>
#include <vector>
#include <map>

#pragma comment(lib,"Ws2_32.lib")



typedef struct {
	sockaddr clientaddr;
	int clientaddrlen;
	SOCKET clientsocket;
}ClientInfo, *PClientInfo;



class TCPServer {
protected:
	std::string ipaddress;
	unsigned short port;
	SOCKET serversocket;
	ADDRINFOA* result;
	bool locked;
	std::vector<ClientInfo> clients;
	int backlog;

public:
	TCPServer();

	TCPServer(std::string ip, unsigned short port, SOCKET serversocket, ADDRINFO* result);

	static bool IsIpAddressValid(std::string ip);
	

	bool bindto();
	bool startlistening(int backlog);
	bool startaccepting();
	bool shalliaccept();

	ClientInfo get_latest_client();
	std::vector<ClientInfo>& get_clients();
		
	std::string get_ipaddress();
	unsigned short get_portnumber();

	void handle_client(SOCKET& clientsocket);

	~TCPServer();

};


std::expected<std::unique_ptr<TCPServer>, int> InitializeTCPServer(std::string ip, unsigned short port);
