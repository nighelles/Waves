#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>

#include "NetworkDefinitions.h"

#define GAME_NETWORK_PORT "4444"

class NetworkController
{
public:
	NetworkController();
	~NetworkController();

	bool Initialize();
	void Shutdown();

	bool SendData(sockaddr_in* address, char* data, int data_size);

	void CloseSocket();

protected:
	SOCKET m_socketHandle;

};

