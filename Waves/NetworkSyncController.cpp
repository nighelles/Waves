#include "NetworkSyncController.h"

#include <stdio.h>
#include <atldef.h>
#include <atlstr.h>

#define MAX_WAIT 1000

NetworkSyncController::NetworkSyncController()
{
	m_isServer = 0;

	m_networkController = 0;

	m_numEntities = 0;

	m_ack = 0;
	m_clientAck = 0;

	m_waitCount = 0;
	m_waiting = false;

	m_currentNetworkState = 0;

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
		OutputDebugString(L"Can't register entity to Network Sync Controller");
		return -1;
	}

	m_numEntities += 1;

	return m_numEntities - 1;
}

bool NetworkSyncController::SyncEntityStates()
{
	float x, y, z, yaw, pitch, roll;
	int entityIndex;

	bool result;

	result = true;

	//sync current entity states
	NetworkState newState;
	for (int i = 0; i != m_numEntities; ++i)
	{
		newState.entities[i].NewNetworkedEntity(m_entities[i]);
	}

	memcpy(&(m_networkStates[m_currentNetworkState]), 0, sizeof(NetworkState));

	m_networkStates[m_currentNetworkState] = newState;

	if (m_isServer)
	{
		if (!m_waiting)
		{
			m_serverMessage.ack = m_ack;
			m_serverMessage.messageType = SERVERSENDSTATE;

			DeltaCompress();

			memcpy(m_serverMessage.data, m_datastream, DATALENGTH);

			result = ((NetworkServer*)m_networkController)->SendDataToClient((char*)&m_serverMessage, sizeof(m_serverMessage));
			m_ack += 1;

			m_waiting = true;
			m_waitCount = 0;
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

			memcpy(m_datastream, m_serverMessage.data, DATALENGTH);

			DeltaUncompress();

			for (int i = 0; i != m_numEntities; ++i)
			{
				m_networkStates[m_clientAck].entities[i].ApplyChanges(m_entities[i]);
			}
		}
	}

	if (!result) OutputDebugString(L"Couldn't sync entity states.\n");

	return result;
}

bool NetworkSyncController::SyncPlayerInput(NetworkedInput* inp)
{
	bool result;

	result = true;

	if (m_isServer)
	{
		char clientData[256];
		int dataSize = sizeof(clientData);

		((NetworkClient*)m_networkController)->GetDataFromServer(clientData, dataSize);

		m_clientMessage = *((ClientNetworkMessage*)clientData);

		if (m_clientMessage.messageType = CLIENTSENDINPUT)
		{
			ClientNetworkMessage* newClientInput = (ClientNetworkMessage*)&m_clientMessage;
			if (m_clientMessage.ack != m_ack - 1)
			{
				m_waitCount += 1;
				if (m_waitCount == MAX_WAIT)
				{
					char msg[100];
					sprintf_s(msg, 100, "FORCE: Client at ack# %d, should be %d \n", m_clientMessage.ack, m_ack - 1);
					OutputDebugString(ATL::CA2W(msg));
					m_waitCount = 0;
					m_waiting = false;
				}
			}
			else {
				m_waiting = false;
				m_waitCount = 0;
			}

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

int NetworkSyncController::DeltaCompress()
{
	int clientStateLocation = m_currentNetworkState + (m_ack - m_clientAck);

	while (!(clientStateLocation < MAXACKDELAY)) clientStateLocation -= MAXACKDELAY;

	char data[DATALENGTH];
	char olddata[DATALENGTH];

	memcpy(data, m_networkStates[0].entities, sizeof(m_networkStates[0].entities));
	memcpy(olddata, m_networkStates[clientStateLocation].entities, sizeof(m_networkStates[clientStateLocation].entities));

	int dataIndex=0;
	memcpy(m_datastream, 0, sizeof(m_datastream));

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
}

bool NetworkSyncController::DeltaUncompress()
{
	char entityCount;
	char dataValue;

	char data[DATALENGTH];

	memcpy(data, m_networkStates[0].entities, sizeof(m_networkStates[0].entities));

	for (int dataIndex = 0; dataIndex != DATALENGTH; dataIndex += 2)
	{
		if (m_datastream[dataIndex] = 0xFF && m_datastream[dataIndex + 1] == 0xFF)
		{
			break;
		}
		else {
			data[m_datastream[dataIndex]] = m_datastream[dataIndex + 1];
		}
	}

	memcpy(m_networkStates[0].entities, data, sizeof(m_networkStates[0].entities));
}

