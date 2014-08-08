#include "PlayerEntity.h"


PlayerEntity::PlayerEntity() : PhysicsEntity()
{
	m_maxWalkSpeed = METERS(7);
	m_jumpStrength = METERS(G_CONST / 2);
	m_airbornControl = 0.5;
	m_runMultiplier = 2.0;
}


PlayerEntity::~PlayerEntity()
{
}

void PlayerEntity::Movement(float x, float y, float z, float dt)
{
	D3DXVECTOR3 walkVec;

	if (m_underwater)
		walkVec = D3DXVECTOR3(x, y, z);
	else
		walkVec = D3DXVECTOR3(x, 0, z);

	D3DXVec3Normalize(&walkVec, &walkVec);

	walkVec *= m_maxWalkSpeed;
	if (m_run) walkVec *= m_runMultiplier;

	if (m_grounded)
	{
		SetVelocity(walkVec.x, m_velocityY, walkVec.z);
	}
	else
	{
		if (m_underwater)
		{
			walkVec *= 0.5;
			ApplyImpulseAccel(walkVec.x, walkVec.y, walkVec.z, dt);
		}
		else 
		{
			// in the air
			walkVec *= m_airbornControl;
			ApplyImpulseAccel(walkVec.x, 0, walkVec.z, dt);
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
