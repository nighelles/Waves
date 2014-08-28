#include "PlayerEntity.h"


PlayerEntity::PlayerEntity() : PhysicsEntity()
{
	m_maxWalkSpeed = METERS(10);
	m_jumpStrength = METERS(G_CONST / 2);
	m_airbornControl = 0.5;
	m_runMultiplier = 2.0;

	type = PLAYER;
}


PlayerEntity::~PlayerEntity()
{
}

void PlayerEntity::Render(GraphicsController* graphics)
{
	graphics->GetEntityModel(m_modelID)->SetLocation(m_locationX, m_locationY, m_locationZ);
	graphics->GetEntityModel(m_modelID)->SetRotation(0, m_rotationY, 0);
}

void PlayerEntity::Movement(float x, float y, float z, float dt)
{
	D3DXVECTOR3 walkVec;
	D3DXMATRIX rot;

	float yaw, pitch, roll;
	yaw = m_rotationY * 0.0174532925f;
	pitch = m_rotationX * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	D3DXMatrixRotationYawPitchRoll(&rot, yaw, 0, 0);

	if (m_underwater)
		walkVec = D3DXVECTOR3(x, y, z);
	else
		walkVec = D3DXVECTOR3(x, 0, z);

	D3DXVec3TransformCoord(&walkVec, &walkVec, &rot);

	//D3DXVec3Normalize(&walkVec, &walkVec);

	walkVec *= m_maxWalkSpeed;
	if (m_run) walkVec *= m_runMultiplier;

	if (m_grounded)
	{
		ApplyImpulseDirect(walkVec.x, m_velocityY, walkVec.z);
	}
	else
	{
		if (m_underwater)
		{
			walkVec *= 0.5;
			ApplyImpulseDirect(walkVec.x, walkVec.y, walkVec.z);
		}
		else 
		{
			// in the air
			walkVec *= m_airbornControl;
			ApplyImpulseDirect(walkVec.x, 0, walkVec.z);
		}
	}
	
}

void PlayerEntity::Stop(float dt)
{
	if (m_grounded)
	{
		if (sqrt(pow(m_velocityX, 2) + pow(m_velocityZ, 2)) < 0.01)
		{
			m_velocityX = 0; m_velocityZ = 0;
		}
		else
		{
			m_velocityX /= 2;
			m_velocityZ /= 2;
		}
		return;
	}
}


void PlayerEntity::Jump(float dt)
{
	if (m_grounded && !m_binded) ApplyImpulseDirect(0, m_jumpStrength, 0);
	if (m_underwater)
	{
		SetVelocity(m_velocityX, m_maxWalkSpeed / 2, m_velocityZ);
	}
}

void PlayerEntity::GetBindLocation(float& x, float& y, float& z)
{
	D3DXVECTOR3 weaponBindLocation;
	D3DXMATRIX rotationMatrix;

	//code reuse :(
	float yaw, pitch, roll;
	yaw = m_rotationY * 0.0174532925f;
	pitch = m_rotationX * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	weaponBindLocation.x = 0.2f;
	weaponBindLocation.y = 0.5f;  // out in front
	weaponBindLocation.z = 1.2f;

	D3DXVec3TransformCoord(&weaponBindLocation, &weaponBindLocation, &rotationMatrix);

	x = m_locationX + weaponBindLocation.x;
	y = m_locationY + weaponBindLocation.y;
	z = m_locationZ + weaponBindLocation.z;
	return;
}
