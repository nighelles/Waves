#include "NetworkClient.h"
#include <stdio.h>
#include <atldef.h>
#include <atlstr.h>

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
	bool foundServer = false;
	int fromLength = sizeof(from);

	int bytes = recvfrom(m_socketHandle, (char*)packet_data, max_packet_size, 0, (sockaddr*)&from, &fromLength);

	if (bytes > 0)
	{
		NetworkMessage *temp = (NetworkMessage*)&packet_data;

		if (temp->messageType == JOINACCEPT)
		{
			foundServer = true;
		}

		// We already have a socket from the super's initialization, just store the address
	}
	return foundServer;
}

bool NetworkClient::GetDataFromServer(ServerNetworkMessage* serverMessage)
{
	unsigned char packet_data[256];
	unsigned int max_packet_size = sizeof(packet_data);

	memset(packet_data, 0, sizeof(packet_data));

	sockaddr_in from;
	int fromLength = sizeof(from);

	int bytes = recvfrom(m_socketHandle, (char*)packet_data, max_packet_size, 0, (sockaddr*)&from, &fromLength);
	
	if (bytes == SOCKET_ERROR)
	{
		OutputDebugString(L"Socket Error\n");
		char msg[100];
		sprintf_s(msg, 100, "Error code: %d\n", WSAGetLastError());
		OutputDebugString(ATL::CA2W(msg));
	}
	
	if (bytes <= 0) return false;

	serverMessage = (ServerNetworkMessage*)&packet_data;


	// ADD timeout
	return true;
}

bool NetworkClient::SendDataToServer(char* data, int datasize)
{
	return SendData((sockaddr_in*)m_serverInfo->ai_addr, data, datasize);
}