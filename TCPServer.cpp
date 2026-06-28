#include "TCPServer.h"

std::expected<std::unique_ptr<TCPServer>, int> InitializeTCPServer(std::string ip, unsigned short port) {

		if (!TCPServer::IsIpAddressValid(ip)) { 
			std::cout << "Invalid IP Address provided\n";
			return std::unexpected<int>(0);
		}


		WSADATA wsadata;
		int res = WSAStartup(MAKEWORD(2, 2), &wsadata);
		if (res != 0) {
			std::cout << "WSAStartup failed: " << res << std::endl;
			return std::unexpected <int>( res);
		}

		ip.push_back('\0');
		ADDRINFOA addrinfo{0}, * result;
		std::string portnumber = std::to_string(port);
		portnumber.push_back('\0');
		addrinfo.ai_family = AF_INET;
		addrinfo.ai_flags = AI_PASSIVE;
		addrinfo.ai_socktype = SOCK_STREAM;
		addrinfo.ai_protocol = IPPROTO_TCP;
		res = getaddrinfo(&ip[0], &portnumber[0], &addrinfo, &result);
		if (res != 0) {
			std::cout << "getaddrinfo failed: " << res << std::endl;
			return std::unexpected<int>(res);
		}

		SOCKET serversocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (serversocket == INVALID_SOCKET) {
			std::cout << "Creating server socket failed: " << WSAGetLastError() << std::endl;
			freeaddrinfo(result);
			WSACleanup();
			return std::unexpected<int>(WSAGetLastError());
		}
	

		return make_unique<TCPServer>(std::move(ip),  port,serversocket,  result);


	}



bool TCPServer::IsIpAddressValid(std::string ip) {

	ip.push_back('\0');
	in_addr inaddr{ 0 };

	return InetPtonA(AF_INET, &ip[0], &inaddr);
}




TCPServer::~TCPServer() {
	//std::cout << "object address: " << this << std::endl;
	std::cout << "Freeing addrinfo and closing serversocket\n";

	if (result != NULL) { freeaddrinfo(result); }

	if (serversocket!=INVALID_SOCKET) { closesocket(serversocket); }

	for (ClientInfo& client : clients) {
		if (client.clientsocket != INVALID_SOCKET) {
			closesocket(client.clientsocket);
		}
	}

	WSACleanup();

}




TCPServer::TCPServer(std::string ip, unsigned short port, SOCKET serversocket, ADDRINFO* result) {
	
	this->backlog = 0;
	this->locked = TRUE;
	this->ipaddress = std::move(ip);
	this->port = port;
	this->serversocket = serversocket;
	this->result = result;
}



bool TCPServer::bindto() {

	int res = bind(serversocket, result->ai_addr, result->ai_addrlen);
	if (res != 0) {
		std::cout << "Bind failed: " << res << std::endl;
		return FALSE;
	}
	//std::cout << "Server socket successfully bound to: " << ipaddress << ":" << port << std::endl;
	return TRUE;

}




std::string TCPServer::get_ipaddress() {
	return this->ipaddress;
}


unsigned short TCPServer::get_portnumber() { return port; }


bool TCPServer::startlistening(int backlog) {
	this->backlog = backlog;
	int res = listen(this->serversocket, backlog);
	if (res != 0) {
		std::cout << "Listening failed: " << WSAGetLastError() << std::endl;
		return FALSE;
	}

	return TRUE;

}

TCPServer::TCPServer(){}

bool TCPServer::startaccepting() {

	sockaddr client{ 0 };
	int clientlen = sizeof(client);
	SOCKET clientsocket = accept(serversocket, &client,&clientlen );
	if (clientsocket == INVALID_SOCKET) {
		std::cout << "accept failed: " << WSAGetLastError() << std::endl;
		return false;
	}

	this->backlog--;
	std::cout << "Decreased backlog to: " << backlog << std::endl;
	clients.push_back({ client, clientlen,clientsocket });

	return TRUE;

}


ClientInfo TCPServer::get_latest_client() { return clients.back(); }

std::vector<ClientInfo>& TCPServer::get_clients() { return clients; }


bool TCPServer::shalliaccept() {
	return this->backlog > 0 ? TRUE: FALSE;
}


void TCPServer::handle_client(SOCKET& clientsocket) {

	std::string msg = "heloo this is server\0";
	int res = send(clientsocket, &msg[0], msg.length(), 0);
	if (res == SOCKET_ERROR) {
		std::cout << "sending to client failed: " << WSAGetLastError() << std::endl;

		return;
	}


	while (true) {


		char recvbuffer[512]{ 0 };
		int res = recv(clientsocket, recvbuffer, 512, 0);
		if (res == SOCKET_ERROR) {
			std::cout << "receiving from client failed: " << WSAGetLastError() << std::endl;
			break;
		}

		std::cout << "[+] Received from client: " << recvbuffer << std::endl;
		std::string recved(recvbuffer);
		if (recved == "quit" || recved == "QUIT") {
			std::cout << "client sent us quit, so quitting\n";
			break;
		}

		
		res = send(clientsocket, &recved[0], recved.length(), 0);
		if (res == SOCKET_ERROR) {
			std::cout << "sending to client failed: " << WSAGetLastError() << std::endl;
			break;
		}

	}



}


