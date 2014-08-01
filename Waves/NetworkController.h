#pragma once

#include <winsock2.h>
#include <ws2tcpip.h>

#define GAME_NETWORK_PORT "4444"

#pragma comment( lib, "ws2_32.lib")

class NetworkController
{
public:
	typedef struct
	{
		bool 	forward;
		bool 	backward;
		bool 	left;
		bool 	right;

		int 	mouseX;
		int		mouseY;
	} PlayerInput;

	typedef struct
	{
		int clientID;
		PlayerInput playerInput;
	} ClientNetworkPacket;

public:
	NetworkController();
	~NetworkController();

	bool Initialize();
	void Shutdown();

	bool SendData(sockaddr_in* address, char* data, int data_size);

	void CloseSocket();

private:
	int m_socketHandle;

};

