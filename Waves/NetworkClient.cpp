#include "NetworkClient.h"


NetworkClient::NetworkClient() : NetworkController()
{
	m_serverInfo = 0;
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

	NetworkMessage joinmsg{ JOINREQUEST };
	SendDataToServer((char*)&joinmsg, sizeof(joinmsg));

	unsigned char packet_data[256];
	unsigned int max_packet_size = sizeof(packet_data);

	sockaddr_in from;
	int fromLength = sizeof(from);

	bool foundServer = false;
	while (!foundServer)
	{

		int bytes = recvfrom(m_socketHandle, (char*)packet_data, max_packet_size, 0, (sockaddr*)&from, &fromLength);
		if (bytes <= 0) continue;

		if (((NetworkMessage*)&packet_data)->messageType == JOINACCEPT)
		{
			foundServer = true;
		}
	}

	// We already have a socket from the super's initialization, just store the address

	return true;
}

bool NetworkClient::SendDataToServer(char* data, int datasize)
{
	return SendData((sockaddr_in*)m_serverInfo->ai_addr, data, datasize);
}