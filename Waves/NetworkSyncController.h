#pragma once

//#include "NetworkController.h"
#include "NetworkServer.h"
#include "NetworkClient.h"

#include "Entity.h"

class NetworkSyncController
{
public:
	NetworkSyncController();
	~NetworkSyncController();

	bool Initialize(bool isServer, NetworkController* networkController);
	void Shutdown();

	int RegisterEntity(Entity* entity);

	bool SyncEntityStates();
	bool SyncPlayerInput(bool& forward, bool& backward, 
						 bool& left, bool& right,
						 int& mouseDX, int& mouseDY);

private:
	bool m_isServer;

	NetworkController* m_networkController;

	int m_numEntities;
	Entity* m_entities[10];

	ClientNetworkMessage m_clientMessage;
	ServerNetworkMessage m_serverMessage;
};

