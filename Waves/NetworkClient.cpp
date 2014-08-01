#include "NetworkClient.h"


NetworkClient::NetworkClient() : NetworkController()
{
}


NetworkClient::~NetworkClient()
{
}

bool NetworkClient::ConnectToServer(char* address)
{
	struct addrinfo hints {0};

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;

	if (getaddrinfo(address, GAME_NETWORK_PORT, &hints, &m_serverInfo) != 0)
		return false;

	// We already have a socket from the super's initialization, just store the address

	return true;
}

bool NetworkClient::SendDataToServer(char* data, int datasize)
{
	SendData((sockaddr_in*)m_serverInfo->ai_addr, data, datasize);
}