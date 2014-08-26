#include "NetworkServer.h"

#include <stdio.h>
#include <atldef.h>
#include <atlstr.h>

NetworkServer::NetworkServer() : NetworkController()
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
	
	int sock = m_socketHandle;

	memset(packet_data, 0, sizeof(packet_data));

	int bytes = recvfrom(sock, (char*)packet_data, max_packet_size, 0, (sockaddr*)&from, &fromLength);

	if (bytes == SOCKET_ERROR && WSAGetLastError() != 10035)
	{
		OutputDebugString(L"Socket Error\n");
		char msg[100];
		sprintf_s(msg,100, "Error code: %d\n", WSAGetLastError());
		OutputDebugString(ATL::CA2W(msg));
	}


	if (bytes > 0)
	{

		NetworkMessage* temp = (NetworkMessage*)&packet_data;
		if (temp->messageType == JOINREQUEST)
		{
			m_clientAddr = from;
			NetworkMessage acceptmsg{ JOINACCEPT };
			SendDataToClient((char*)&acceptmsg, sizeof(acceptmsg));
			foundClient = true;
		}
	}
	// ADD timeout
	return foundClient;
}

bool NetworkServer::GetDataFromClient(char* data, int datasize)
{
	sockaddr_in from;
	int fromLength = sizeof(from);

	int bytes = recvfrom(m_socketHandle, (char*)data, datasize, 0, (sockaddr*)&from, &fromLength);
	if (bytes <= 0) return false;
	
	// ADD timeout
	return true;
}

bool NetworkServer::SendDataToClient(char* data, int datasize)
{
	return SendData((sockaddr_in*)&m_clientAddr, data, datasize);
}