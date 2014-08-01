#include "NetworkController.h"

NetworkController::NetworkController()
{
	m_socketHandle = 0;
}

NetworkController::~NetworkController()
{
}

bool NetworkController::Initialize()
{
	bool result;
	WSADATA WsaData;
	
	addrinfo hints {0};
	addrinfo *server_info, *p;

	result = WSAStartup(MAKEWORD(2, 2), &WsaData);
	if (!result) return false;

	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;

	if (getaddrinfo(NULL, GAME_NETWORK_PORT, &hints, &server_info) != 0)
		return false;

	p = server_info;
	// could be multiple entries, probably should fix this
	m_socketHandle = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
	if (m_socketHandle == -1) return false;

	if (bind(m_socketHandle, p->ai_addr, p->ai_addrlen) == -1) return false;

	DWORD nonBlocking = 1;
	if (ioctlsocket(m_socketHandle, FIONBIO, &nonBlocking) != 0)
		return false;

	freeaddrinfo(server_info);

	return true;
}

bool NetworkController::SendData(sockaddr_in* address, char* data, int data_size)
{
	int sent_bytes;

	sent_bytes =
		sendto(m_socketHandle,
		(const char*)data,
		data_size, 0,
		(sockaddr*)address, 
		sizeof(sockaddr_in));

	if (sent_bytes != data_size)
		return false;

	return true;
}


void NetworkController::CloseSocket()
{
	closesocket(m_socketHandle);
}

void NetworkController::Shutdown()
{
	WSACleanup();
}
