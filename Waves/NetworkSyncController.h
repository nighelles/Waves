#pragma once

//#include "NetworkController.h"
#include "NetworkServer.h"
#include "NetworkClient.h"

#include "PhysicsEntity.h"

class NetworkSyncController
{
private:
	typedef struct
	{
		NetworkedEntity entities[MAXNETWORKENTITIES];
	} NetworkState;

public:
	NetworkSyncController();
	~NetworkSyncController();

	bool Initialize(bool isServer, NetworkController* networkController);
	void Shutdown();

	int RegisterEntity(PhysicsEntity* entity);

	bool SyncEntityStates(float dt);
	bool SyncPlayerInput(NetworkedInput* inp, int& playerNum);

	int DeltaCompress();
	bool DeltaUncompress();
	void NewNetworkedEntity(NetworkedEntity *net, PhysicsEntity* ent);
	void ApplyChanges(NetworkedEntity net, PhysicsEntity* ent);

private:
	bool m_isServer;

	NetworkController* m_networkController;

	UINT32 m_ack, m_clientAck;

	float m_waitTime;
	float m_packetSpacing;
	bool m_waiting;
	int m_goodPackets;

	int m_numEntities;
	PhysicsEntity* m_entities[MAXNETWORKENTITIES];

	NetworkState m_networkStates[MAXACKDELAY];
	int m_currentNetworkState;

	int m_datastream[STREAMLENGTH];

	ClientNetworkMessage m_clientMessage;
	ServerNetworkMessage m_serverMessage;
};

