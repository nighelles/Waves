#pragma once

#include "NetworkController.h"

class NetworkServer : public NetworkController
{
public:
	NetworkServer();
	~NetworkServer();

	bool WaitForClient();
	bool SendDataToClient(char* data, int datasize);

private:
	sockaddr_in m_clientAddr;
};

