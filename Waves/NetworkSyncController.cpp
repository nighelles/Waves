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

	if (m_isServer)
	{
		m_serverMessage.ack = m_ack;
		m_serverMessage.messageType = SERVERSENDSTATE;
		m_serverMessage.numEntityStates = m_numEntities;

		for (int i = 0; i != m_numEntities; ++i)
		{
			m_entities[i]->GetLocation(x, y, z);
			m_entities[i]->GetRotation(yaw, pitch, roll);

			m_serverMessage.entityStates[i].position = D3DXVECTOR3(x, y, z);
			m_serverMessage.entityStates[i].velocity = m_entities[i]->GetVelocity();
			m_serverMessage.entityStates[i].rotation = D3DXVECTOR3(yaw, pitch, roll);


			m_serverMessage.entityStates[i].physicsEntityID = i;
		}

		result = ((NetworkServer*)m_networkController)->SendDataToClient((char*)&m_serverMessage, sizeof(m_serverMessage));
		m_ack += 1;
	}
	else 
	{
		((NetworkClient*)m_networkController)->GetDataFromServer(&m_serverMessage);

		if (m_serverMessage.messageType == SERVERSENDSTATE)
		{
			if (m_serverMessage.ack > m_ack + 1) OutputDebugString(L"Lost some game state");
			m_ack = m_serverMessage.ack;

			for (int i = 0; i != m_serverMessage.numEntityStates; ++i)
			{
				entityIndex = m_serverMessage.entityStates[i].physicsEntityID;
				x = m_serverMessage.entityStates[i].position.x;
				y = m_serverMessage.entityStates[i].position.y;
				z = m_serverMessage.entityStates[i].position.z;
				yaw = m_serverMessage.entityStates[i].rotation.x;
				pitch = m_serverMessage.entityStates[i].rotation.y;
				roll = m_serverMessage.entityStates[i].rotation.z;

				m_entities[entityIndex]->SetVelocity(
					m_serverMessage.entityStates[i].velocity.x,
					m_serverMessage.entityStates[i].velocity.y,
					m_serverMessage.entityStates[i].velocity.z);

				m_entities[entityIndex]->SetLocation(x, y, z);
				m_entities[entityIndex]->SetRotation(yaw, pitch, roll);
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
		((NetworkServer*)m_networkController)->GetDataFromClient(&m_clientMessage);

		if (m_clientMessage.messageType = CLIENTSENDINPUT)
		{
			if (m_clientMessage.ack != m_ack - 1)
			{
				OutputDebugString(L"Client lost game state");
			}

			inp->keys[Network_W] = m_clientMessage.input.keys[Network_W];
			inp->keys[Network_A] = m_clientMessage.input.keys[Network_A];
			inp->keys[Network_S] = m_clientMessage.input.keys[Network_S];
			inp->keys[Network_D] = m_clientMessage.input.keys[Network_D];
			inp->keys[Network_SHIFT] = m_clientMessage.input.keys[Network_SHIFT];
			inp->keys[Network_CONTROL] = m_clientMessage.input.keys[Network_CONTROL];
			inp->keys[Network_SPACE] = m_clientMessage.input.keys[Network_SPACE];

			inp->mouseDX = m_clientMessage.input.mouseDX;
			inp->mouseDY = m_clientMessage.input.mouseDY;
		}
	}
	else
	{
		m_clientMessage.messageType = CLIENTSENDINPUT;
		m_clientMessage.ack = m_ack;

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

