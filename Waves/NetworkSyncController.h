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

	void NextNetworkState();
	int GetIndexForAckDifference(int ackNew, int ackOld);

	bool DeltaUncompress(int fromAck);
	void NewNetworkedEntity(NetworkedEntity *net, PhysicsEntity* ent);
	void ApplyChanges(NetworkedEntity current, NetworkedEntity net, PhysicsEntity* ent, float interpolation);
	bool DoStatesDiffer(NetworkState *a, NetworkState *b, int num);

private:
	bool m_isServer;

	NetworkController* m_networkController;

	UINT32 m_ack, m_clientAck;

	int m_serverAtIndex;

	float m_waitTime;
	float m_packetSpacing;
	bool m_waiting;
	int m_goodPackets;
	int m_goodThreshold;
	bool m_checkAgainstNetwork;

	float m_errorInterpolation;
	float m_errorInterpolationTime;
	bool m_correctingError;

	bool m_sendInput;
	NetworkedInput m_inpAcc;

	int m_numEntities;
	PhysicsEntity* m_entities[MAXNETWORKENTITIES];

	NetworkState m_networkStates[MAXACKDELAY];
	NetworkState m_predictedStates[MAXACKDELAY];
	int m_currentNetworkState;

	int m_datastream[STREAMLENGTH];

	ClientNetworkMessage m_clientMessage;
	ServerNetworkMessage m_serverMessage;
};

