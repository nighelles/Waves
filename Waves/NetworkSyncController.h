#pragma once

//#include "NetworkController.h"
#include "NetworkServer.h"
#include "NetworkClient.h"

#include "PhysicsEntity.h"

class NetworkSyncController
{
private:
	struct NetworkState
	{
		NetworkedEntity *entities;
	};

public:
	NetworkSyncController();
	~NetworkSyncController();

	bool Initialize(bool isServer, NetworkController* networkController);
	void Shutdown();

	int RegisterEntity(PhysicsEntity* entity);

	bool SyncEntityStates();
	bool SyncPlayerInput(NetworkedInput* inp);

	int DeltaCompress();
	bool DeltaUncompress();

private:
	bool m_isServer;

	NetworkController* m_networkController;

	UINT32 m_ack, m_clientAck;

	int m_waitCount;
	bool m_waiting;

	int m_numEntities;
	PhysicsEntity* m_entities[MAXNETWORKENTITIES];

	NetworkState* m_networkStates;
	int m_currentNetworkState;

	int m_datastream[DATALENGTH/sizeof(int)];

	ClientNetworkMessage m_clientMessage;
	ServerNetworkMessage m_serverMessage;
};

