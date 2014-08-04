#include "NetworkSyncController.h"

NetworkSyncController::NetworkSyncController()
{
	m_isServer = 0;

	m_networkController = 0;

	m_numEntities = 0;

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

int NetworkSyncController::RegisterEntity(Entity* entity)
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

	if (m_isServer)
	{
		m_serverMessage.messageType = SERVERSENDSTATE;
		m_serverMessage.numEntityStates = m_numEntities;

		for (int i = 0; i != m_numEntities; ++i)
		{
			m_entities[i]->GetLocation(x, y, z);
			m_entities[i]->GetRotation(yaw, pitch, roll);

			m_serverMessage.entityStates[i].physicsEntityID = i;
			m_serverMessage.entityStates[i].x = x;
			m_serverMessage.entityStates[i].y = y;
			m_serverMessage.entityStates[i].z = z;
			m_serverMessage.entityStates[i].yaw = yaw;
			m_serverMessage.entityStates[i].pitch = pitch;
			m_serverMessage.entityStates[i].roll = roll;
		}

		result = ((NetworkServer*)m_networkController)->SendDataToClient((char*)&m_serverMessage, sizeof(m_serverMessage));
	}
	else 
	{
		((NetworkClient*)m_networkController)->GetDataFromServer(&m_serverMessage);

		if (m_serverMessage.messageType == SERVERSENDSTATE)
		{
			for (int i = 0; i != m_serverMessage.numEntityStates; ++i)
			{
				entityIndex = m_serverMessage.entityStates[i].physicsEntityID;
				x = m_serverMessage.entityStates[i].x;
				y = m_serverMessage.entityStates[i].y;
				z = m_serverMessage.entityStates[i].z;
				yaw = m_serverMessage.entityStates[i].yaw;
				pitch = m_serverMessage.entityStates[i].pitch;
				roll = m_serverMessage.entityStates[i].roll;

				m_entities[entityIndex]->SetLocation(x, y, z);
				m_entities[entityIndex]->SetRotation(yaw, pitch, roll);
			}
		}
	}

	if (!result) OutputDebugString(L"Couldn't sync entity states.\n");

	return result;
}

bool NetworkSyncController::SyncPlayerInput(bool& forward, bool& backward, bool& left, bool& right, int& mouseDX, int& mouseDY)
{
	bool result;

	result = true;

	if (m_isServer)
	{
		((NetworkServer*)m_networkController)->GetDataFromClient(&m_clientMessage);

		if (m_clientMessage.messageType = CLIENTSENDINPUT)
		{
			forward = m_clientMessage.forward;
			backward = m_clientMessage.backward;
			left = m_clientMessage.left;
			right = m_clientMessage.right;
			mouseDX = m_clientMessage.mouseDX;
			mouseDY = m_clientMessage.mouseDY;
		}
	}
	else
	{
		m_clientMessage.messageType = CLIENTSENDINPUT;

		m_clientMessage.forward = forward;
		m_clientMessage.backward = backward;
		m_clientMessage.left = left;
		m_clientMessage.right = right;
		m_clientMessage.mouseDX = mouseDX;
		m_clientMessage.mouseDY = mouseDY;

		result = ((NetworkClient*)m_networkController)->SendDataToServer((char*)&m_clientMessage, sizeof(m_clientMessage));
	}

	if (!result) OutputDebugString(L"Could not sync player input.\n");
	return result;
}

