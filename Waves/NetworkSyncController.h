#pragma once

//#include "NetworkController.h"
#include "NetworkServer.h"
#include "NetworkClient.h"

#include "PhysicsEntity.h"

class NetworkSyncController
{

public:
	NetworkSyncController();
	~NetworkSyncController();

	bool Initialize(bool isServer, NetworkController* networkController);
	void Shutdown();

	int RegisterEntity(PhysicsEntity* entity);

	bool SyncEntityStates();
	bool SyncPlayerInput(NetworkedInput* inp);

private:
	bool m_isServer;

	NetworkController* m_networkController;

	UINT32 m_ack;

	int m_waitCount;
	bool m_waiting;

	int m_numEntities;
	PhysicsEntity* m_entities[3];

	ClientNetworkMessage m_clientMessage;
	ServerNetworkMessage m_serverMessage;
};

