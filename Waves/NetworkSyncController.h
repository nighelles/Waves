#pragma once

#include "Entity.h"

//#include "NetworkController.h"
#include "NetworkServer.h"
#include "NetworkClient.h"

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
						 float& mouseDX, float& mouseDY);

private:
	bool m_isServer;

	NetworkController* m_networkController;

	int m_numEntities;
	Entity* m_entities[10];

	ClientNetworkMessage m_clientMessage;
	ServerNetworkMessage m_serverMessage;
};

