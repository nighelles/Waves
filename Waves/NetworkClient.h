#pragma once

#include "NetworkController.h"

class NetworkClient : public NetworkController
{
public:
	NetworkClient();
	~NetworkClient();

	bool ConnectToServer(char* address);
	bool SendDataToServer(char* data, int datasize);
	bool GetDataFromServer(char* data, int datasize);

private:
	addrinfo *m_serverInfo;
};

