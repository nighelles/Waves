#include "NetworkServer.h"


NetworkServer::NetworkServer()
{
}


NetworkServer::~NetworkServer()
{
}

bool NetworkServer::WaitForClient()
{
	unsigned char packet_data[256];
	unsigned int max_packet_size = sizeof(packet_data);

	sockaddr_in from;
	int fromLength = sizeof(from);

	bool foundClient = false;
	
	int bytes = recvfrom(m_socketHandle, (char*)packet_data, max_packet_size, 0, (sockaddr*)&from, &fromLength);

	if (((NetworkMessage*)&packet_data)->messageType == JOINREQUEST)
	{
		m_clientAddr = from;
		NetworkMessage acceptmsg{ JOINACCEPT };
		SendDataToClient((char*)&acceptmsg, sizeof(acceptmsg));
		foundClient = true;
	}
	
	// ADD timeout
	return foundClient;
}

bool NetworkServer::GetDataFromClient(ClientNetworkMessage* clientMessage)
{
	unsigned char packet_data[256];
	unsigned int max_packet_size = sizeof(packet_data);

	sockaddr_in from;
	int fromLength = sizeof(from);

	int bytes = recvfrom(m_socketHandle, (char*)packet_data, max_packet_size, 0, (sockaddr*)&from, &fromLength);
	if (bytes <= 0) return false;

	if (((NetworkMessage*)&packet_data)->messageType == CLIENTSENDINPUT)
	{
		clientMessage = (ClientNetworkMessage*)&packet_data;
	}
	else 
	{
		return false;
	}
	
	// ADD timeout
	return true;
}

bool NetworkServer::SendDataToClient(char* data, int datasize)
{
	return SendData((sockaddr_in*)&m_clientAddr, data, datasize);
}