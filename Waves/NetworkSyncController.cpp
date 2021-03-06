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
	m_packetSpacing = 0.05;
	m_goodPackets = 0;
	m_goodThreshold = 5;

	m_errorInterpolation = 0.0f;
	m_errorInterpolationTime = 0.0f;
	m_correctingError = false;

	m_checkAgainstNetwork = false;

	m_currentNetworkState = 0;

	m_sendInput = false;
	NetworkedInput m_inp = {0};

	NetworkState m_networkStates[MAXACKDELAY] = {0};
	NetworkState m_predictedStates[MAXACKDELAY] = {0};
	
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
		m_sendInput = true;
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

			if (m_clientAck > m_ack - 5)
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
			m_sendInput = true;

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

			int newIndex = GetIndexForAckDifference(m_clientAck, m_serverMessage.clientAck);

			if (m_serverAtIndex != newIndex)
			{
				memcpy(
					&(m_networkStates[newIndex]), 
					&(m_networkStates[m_serverAtIndex]), 
					sizeof(m_networkStates[m_serverAtIndex]));

				m_checkAgainstNetwork = true;
				m_serverAtIndex = newIndex;
			}

			DeltaUncompress(m_serverMessage.clientAck);

			if (m_checkAgainstNetwork)
			{
				if (DoStatesDiffer(
					&m_networkStates[m_serverAtIndex],
					&m_predictedStates[m_currentNetworkState],
					m_numEntities))
				{
					m_correctingError = true;
					m_errorInterpolationTime = 0.0f;
					m_errorInterpolation = 0.0f;
					OutputDebugString(L"Had to change prediction.\n");
				}
				else {
					OutputDebugString(L"Got to keep prediction. \n");
				}

				m_checkAgainstNetwork = false;
			}
		}
		if (m_correctingError)
		{
			OutputDebugString(L"Changing prediction.\n");
			for (int i = 0; i != m_numEntities; ++i)
			{
				m_errorInterpolationTime += dt;
				m_errorInterpolation = m_errorInterpolationTime / ERROR_INTERPOLATION_TIME;

				if (m_errorInterpolation > 1.0f) {
					m_errorInterpolation = 1.0f;
					m_correctingError = false;
				}
				ApplyChanges(m_predictedStates[m_serverAtIndex].entities[i],
					m_networkStates[m_serverAtIndex].entities[i],
					m_entities[i], m_errorInterpolation);
			}
		}
	}

	if (!result) OutputDebugString(L"Couldn't sync entity states.\n");

	return result;
}

bool NetworkSyncController::DoStatesDiffer(NetworkState *a, NetworkState *b, int num)
{
	float thresh = 0.01;
	float rotthresh = 1;
	for (int i = 0; i != num; ++i)
	{
		if (fabs(a->entities[i].x - b->entities[i].x) > thresh) return true;
		if (fabs(a->entities[i].y - b->entities[i].y) > thresh) return true;
		if (fabs(a->entities[i].z - b->entities[i].z) > thresh) return true;
		if (fabs(a->entities[i].vx - b->entities[i].vx) > thresh) return true;
		if (fabs(a->entities[i].vy - b->entities[i].vy) > thresh) return true;
		if (fabs(a->entities[i].vz - b->entities[i].vz) > thresh) return true;
		if (fabs(a->entities[i].rx - b->entities[i].rx) > rotthresh) return true;
		if (fabs(a->entities[i].ry - b->entities[i].ry) > rotthresh) return true;
		if (fabs(a->entities[i].rz - b->entities[i].rz) > rotthresh) return true;
	}
	return false;
}

bool NetworkSyncController::SyncPlayerInputServer(NetworkedInput* inp, int& playerNum)
{
	bool result;

	result = true;
	
	char clientData[256];
	int dataSize = sizeof(clientData);

	((NetworkServer*)m_networkController)->GetDataFromClient(clientData, dataSize);

	m_clientMessage = *((ClientNetworkMessage*)clientData);

	if (m_clientMessage.messageType == CLIENTSENDINPUT)
	{
		ClientNetworkMessage* newClientInput = (ClientNetworkMessage*)&m_clientMessage;

		memcpy(&m_inp, &(newClientInput->input), sizeof(NetworkedInput));

		playerNum = m_clientMessage.playerNumber;

		m_clientAck = m_clientMessage.ack;


		inp->keys[Network_W] = m_inp.keys[Network_W];
		inp->keys[Network_A] = m_inp.keys[Network_A];
		inp->keys[Network_S] = m_inp.keys[Network_S];
		inp->keys[Network_D] = m_inp.keys[Network_D];
		inp->keys[Network_SHIFT] = m_inp.keys[Network_SHIFT];
		inp->keys[Network_CONTROL] = m_inp.keys[Network_CONTROL];
		inp->keys[Network_SPACE] = m_inp.keys[Network_SPACE];
		   
		inp->mouseDY = m_inp.mouseDY;
		inp->mouseDX = m_inp.mouseDX;

		inp->mouse[0] = m_inp.mouse[0];
		inp->mouse[1] = m_inp.mouse[1];
	}

	if (!result) OutputDebugString(L"Could not sync player input.\n");
	return result;
}

bool NetworkSyncController::SyncPlayerInputClient(NetworkedInput inp, int& playerNum)
{
	bool result;

	result = true;

	m_clientMessage.messageType = CLIENTSENDINPUT;
	m_clientMessage.ack = m_clientAck;

	m_clientMessage.playerNumber = playerNum;

	m_clientMessage.input.keys[Network_W] = inp.keys[Network_W];
	m_clientMessage.input.keys[Network_A] = inp.keys[Network_A];
	m_clientMessage.input.keys[Network_S] = inp.keys[Network_S];
	m_clientMessage.input.keys[Network_D] = inp.keys[Network_D];
	m_clientMessage.input.keys[Network_SHIFT] = inp.keys[Network_SHIFT];
	m_clientMessage.input.keys[Network_CONTROL] = inp.keys[Network_CONTROL];
	m_clientMessage.input.keys[Network_SPACE] = inp.keys[Network_SPACE];
	m_clientMessage.input.mouseDX = inp.mouseDX;
	m_clientMessage.input.mouseDY = inp.mouseDY;
	m_clientMessage.input.mouse[0] = inp.mouse[0];
	m_clientMessage.input.mouse[1] = inp.mouse[1];

	result = ((NetworkClient*)m_networkController)->SendDataToServer((char*)&m_clientMessage, sizeof(m_clientMessage));

	m_sendInput = false;

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
	if (m_currentNetworkState >= MAXACKDELAY)
	{
		m_currentNetworkState -= MAXACKDELAY;
	}
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

bool NetworkSyncController::DeltaUncompress(int fromAck)
{
	char entityCount;
	char dataValue;

	char data[DATALENGTH];

	int oldindex = GetIndexForAckDifference(m_clientAck, fromAck);

	memcpy(data, m_networkStates[oldindex].entities, sizeof(m_networkStates[oldindex].entities));

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

	memcpy(m_networkStates[oldindex].entities, data, sizeof(m_networkStates[oldindex].entities));

	return true;
}

void NetworkSyncController::NewNetworkedEntity(NetworkedEntity *net, PhysicsEntity* ent)
{
	float x, y, z, vx, vy, vz, rx, ry, rz;
	ent->GetLocation(x, y, z);
	ent->GetVelocity(vx, vy, vz);
	ent->GetRotation(rx, ry, rz);
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
void NetworkSyncController::ApplyChanges(
	NetworkedEntity current, NetworkedEntity net,
	PhysicsEntity* ent, float interpolation)
{
	float x, y, z;
	float vx, vy, vz;
	float rx, ry, rz;
	x = net.x  *(interpolation)+current.x  *(1.0f - interpolation);
	y = net.y  *(interpolation)+current.y  *(1.0f - interpolation);
	z = net.z  *(interpolation)+current.z  *(1.0f - interpolation);
	vx = net.vx *(interpolation)+current.vx *(1.0f - interpolation);
	vy = net.vy *(interpolation)+current.vy *(1.0f - interpolation);
	vz = net.vz *(interpolation)+current.vz *(1.0f - interpolation);
	rx = net.rx *(interpolation)+current.rx *(1.0f - interpolation);
	ry = net.ry *(interpolation)+current.ry *(1.0f - interpolation);
	rz = net.rz *(interpolation)+current.rz *(1.0f - interpolation);
	ent->SetLocation(x, y, z);
	ent->SetVelocity(vx, vy, vz);
	ent->SetRotation(rx, ry, rz);
}