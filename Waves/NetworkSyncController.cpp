#include "NetworkSyncController.h"

#include <stdio.h>
#include <atldef.h>
#include <atlstr.h>

#include <math.h>

NetworkSyncController::NetworkSyncController()
{
	m_isServer = 0;

	m_networkController = 0;

	m_numEntities = 0;

	m_ack = 0;
	m_clientAck = -1;

	m_waitTime = 0;
	m_waiting = false;
	m_packetSpacing = 0.2;
	m_goodPackets = 0;
	m_goodThreshold = 5;

	m_currentNetworkState = 0;

	NetworkState m_networkStates[MAXACKDELAY] = {};
	NetworkState m_predictedStates[MAXACKDELAY] = {};
	
	m_datastream[STREAMLENGTH] = { 0 };

	return;
}

NetworkSyncController::~NetworkSyncController()
{
}

bool NetworkSyncController::Initialize(bool isServer, NetworkController* networkController)
{
	m_isServer = isServer;

	m_networkController = networkController;

	return true;
}

void NetworkSyncController::Shutdown()
{
	if (m_networkController)
	{
		m_networkController->Shutdown(); 
		delete m_networkController;
		m_networkController = 0;
	}
	for (int i = 0; i != m_numEntities; ++i)
	{
		m_entities[i] = 0;
	}

	return;
}

int NetworkSyncController::RegisterEntity(PhysicsEntity* entity)
{
	m_entities[m_numEntities] = entity;
	if (!m_entities[m_numEntities])
	{
		OutputDebugString(L"Can't register entity to Network Sync Controller\n");
		return -1;
	}

	m_numEntities += 1;

	return m_numEntities - 1;
}

bool NetworkSyncController::SyncEntityStates(float dt)
{
	float x, y, z, yaw, pitch, roll;
	int entityIndex;

	bool result;

	result = true;

	m_waitTime += dt;

	if (m_waitTime > m_packetSpacing)
	{
		m_waitTime -= m_packetSpacing;
		m_waiting = false;
	}

	if (m_isServer)
	{
		if (!m_waiting)
		{
			//sync current entity states
			NetworkState newState;

			for (int i = 0; i != m_numEntities; ++i)
			{
				NewNetworkedEntity(&(newState.entities[i]), m_entities[i]);
			}

			NextNetworkState();

			memcpy(&(m_networkStates[m_currentNetworkState]), &newState, sizeof(NetworkState));

			m_serverMessage.ack = m_ack;
			m_serverMessage.clientAck = m_clientAck;
			m_serverMessage.messageType = SERVERSENDSTATE;

			DeltaCompress();

			memcpy(m_serverMessage.data, (char*)m_datastream, DATALENGTH);

			result = ((NetworkServer*)m_networkController)->SendDataToClient((char*)&m_serverMessage, sizeof(m_serverMessage));
			m_ack += 1;

			m_waiting = true;

			if (m_clientAck = m_ack - 1)
			{
				// This was a good packet
				m_goodPackets += 1;
				if (m_goodPackets > m_goodThreshold)
				{
					m_packetSpacing = 0.05;
					m_goodPackets = m_goodThreshold;

					OutputDebugString(L"Moving to faster packets\n");
				}
			}
			else {
				m_goodPackets -= 1;
				if (m_goodPackets < -m_goodThreshold)
				{
					m_goodPackets = -m_goodThreshold;
					m_packetSpacing = .3;

					OutputDebugString(L"Moving to slower packets\n");
				}
			}
		}
	}
	else 
	{
		char serverData[sizeof(ServerNetworkMessage)];
		int dataSize = sizeof(serverData);

		((NetworkClient*)m_networkController)->GetDataFromServer(serverData, dataSize);

		m_serverMessage = *((ServerNetworkMessage*)serverData);

		if (m_serverMessage.messageType == SERVERSENDSTATE)
		{
			m_clientAck = m_serverMessage.ack;

			//Create new state from what WE think it should be
			NetworkState newState;

			for (int i = 0; i != m_numEntities; ++i)
			{
				NewNetworkedEntity(&(newState.entities[i]), m_entities[i]);
			}

			NextNetworkState();

			memcpy(&(m_predictedStates[m_currentNetworkState]), &newState, sizeof(NetworkState));

			// Then look at what the server sent us
			memcpy(m_datastream, (char*)m_serverMessage.data, DATALENGTH);

			DeltaUncompress();

			int serverAtIndex = GetIndexForAckDifference(m_clientAck, m_serverMessage.clientAck);
			if (DoStatesDiffer(
				&m_networkStates[m_currentNetworkState],
				&m_predictedStates[serverAtIndex],
				m_numEntities))
			{
				for (int i = 0; i != m_numEntities; ++i)
				{
					ApplyChanges(m_networkStates[m_currentNetworkState].entities[i], m_entities[i]);
				}
			}
		}
	}

	if (!result) OutputDebugString(L"Couldn't sync entity states.\n");

	return result;
}

bool NetworkSyncController::DoStatesDiffer(NetworkState *a, NetworkState *b, int num)
{
	return memcmp(a, b, num*sizeof(NetworkState))!=0;
}

bool NetworkSyncController::SyncPlayerInput(NetworkedInput* inp, int& playerNum)
{
	bool result;

	result = true;
	
	if (m_isServer)
	{
		char clientData[256];
		int dataSize = sizeof(clientData);

		((NetworkClient*)m_networkController)->GetDataFromServer(clientData, dataSize);

		m_clientMessage = *((ClientNetworkMessage*)clientData);

		if (m_clientMessage.messageType == CLIENTSENDINPUT)
		{
			ClientNetworkMessage* newClientInput = (ClientNetworkMessage*)&m_clientMessage;

			playerNum = m_clientMessage.playerNumber;

			m_clientAck = m_clientMessage.ack;

			bool w, a, s, d;

			w = inp->keys[Network_W] = newClientInput->input.keys[Network_W];
			a = inp->keys[Network_A] = newClientInput->input.keys[Network_A];
			s = inp->keys[Network_S] = newClientInput->input.keys[Network_S];
			d = inp->keys[Network_D] = newClientInput->input.keys[Network_D];
			inp->keys[Network_SHIFT] = newClientInput->input.keys[Network_SHIFT];
			inp->keys[Network_CONTROL] = newClientInput->input.keys[Network_CONTROL];
			inp->keys[Network_SPACE] = newClientInput->input.keys[Network_SPACE];

			inp->mouseDX = newClientInput->input.mouseDX;
			inp->mouseDY = newClientInput->input.mouseDY;
		}
	}
	else
	{
		m_clientMessage.messageType = CLIENTSENDINPUT;
		m_clientMessage.ack = m_clientAck;

		m_clientMessage.playerNumber = playerNum;

		m_clientMessage.input.keys[Network_W] = inp->keys[Network_W];
		m_clientMessage.input.keys[Network_A] = inp->keys[Network_A];
		m_clientMessage.input.keys[Network_S] = inp->keys[Network_S];
		m_clientMessage.input.keys[Network_D] = inp->keys[Network_D];
		m_clientMessage.input.keys[Network_SHIFT] = inp->keys[Network_SHIFT];
		m_clientMessage.input.keys[Network_CONTROL] = inp->keys[Network_CONTROL];
		m_clientMessage.input.keys[Network_SPACE] = inp->keys[Network_SPACE];

		m_clientMessage.input.mouseDX = inp->mouseDX;
		m_clientMessage.input.mouseDY = inp->mouseDY;

		result = ((NetworkClient*)m_networkController)->SendDataToServer((char*)&m_clientMessage, sizeof(m_clientMessage));
	}

	if (!result) OutputDebugString(L"Could not sync player input.\n");
	return result;
}

int NetworkSyncController::GetIndexForAckDifference(int ackNew, int ackOld)
{
	int loc = m_currentNetworkState - (ackNew - ackOld);

	while ((loc < 0)) loc += MAXACKDELAY;

	return loc;
};

void NetworkSyncController::NextNetworkState()
{
	m_currentNetworkState += 1;
	if (m_currentNetworkState >= MAXACKDELAY) m_currentNetworkState -= MAXACKDELAY;

	return;
}

int NetworkSyncController::DeltaCompress()
{
	int clientStateLocation = GetIndexForAckDifference(m_ack, m_clientAck);

	char data[DATALENGTH];
	char olddata[DATALENGTH];

	memcpy(data, m_networkStates[m_currentNetworkState].entities, sizeof(m_networkStates[0].entities));
	memcpy(olddata, m_networkStates[clientStateLocation].entities, sizeof(m_networkStates[clientStateLocation].entities));

	int dataIndex=0;
	memset(m_datastream, 0, sizeof(m_datastream));

	for (int offset = 0; offset != DATALENGTH; ++offset)
	{
		if (data[offset] != olddata[offset])
		{
			m_datastream[dataIndex] = offset;
			m_datastream[dataIndex + 1] = data[offset];
			dataIndex += 2;
		}
	}
	m_datastream[dataIndex] = 0xFF;
	m_datastream[dataIndex + 1] = 0xFF;

	return true;
}

bool NetworkSyncController::DeltaUncompress()
{
	char entityCount;
	char dataValue;

	char data[DATALENGTH];

	memcpy(data, m_networkStates[m_currentNetworkState-1].entities, sizeof(m_networkStates[m_currentNetworkState-1].entities));

	for (int dataIndex = 0; dataIndex != DATALENGTH; dataIndex += 2)
	{
		if (m_datastream[dataIndex] == 0xFF && m_datastream[dataIndex + 1] == 0xFF)
		{
			break;
		}
		else {
			data[m_datastream[dataIndex]] = m_datastream[dataIndex + 1];
		}
	}

	memcpy(m_networkStates[m_currentNetworkState].entities, data, sizeof(m_networkStates[m_currentNetworkState].entities));

	return true;
}

void NetworkSyncController::NewNetworkedEntity(NetworkedEntity *net, PhysicsEntity* ent)
{
	float x, y, z, vx, vy, vz, rx, ry, rz;
	ent->GetLocation(x, y, z);
	ent->GetVelocity(vx, vy, vz);
	ent->GetRotation(rx, ry, rz);
	if (fabs(x) < 0.0001) x = 0;
	if (fabs(y) < 0.0001) y = 0;
	if (fabs(z) < 0.0001) z = 0;
	if (fabs(vx) < 0.0001) vx = 0;
	if (fabs(vy) < 0.0001) vy = 0;
	if (fabs(vz) < 0.0001) vz = 0;
	if (fabs(rx) < 0.0001) rx = 0;
	if (fabs(ry) < 0.0001) ry = 0;
	if (fabs(rz) < 0.0001) rz = 0;
	net->x = x; 
	net->y = y;
	net->z = z;
	net->vx = vx;
	net->vy = vy;
	net->vz = vz;
	net->rx = rx;
	net->ry = ry;
	net->rz = rz;

	return;
}
void NetworkSyncController::ApplyChanges(NetworkedEntity net, PhysicsEntity* ent)
{
	ent->SetLocation(net.x, net.y, net.z);
	ent->SetVelocity(net.vx, net.vy, net.vz);
	ent->SetRotation(net.rx, net.ry, net.rz);
}