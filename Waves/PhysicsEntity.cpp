#include "PhysicsEntity.h"


PhysicsEntity::PhysicsEntity() : Entity()
{
	m_modelID = 0;
}


PhysicsEntity::~PhysicsEntity()
{
}

bool PhysicsEntity::InitializeModel(GraphicsController* graphics, char* modelFilename, WCHAR* textureFilename)
{
	bool result;

	result = graphics->InitializeEntityModel(modelFilename, textureFilename);
	if (result == -1)
		return false;
	else
		m_modelID = result;

	return true;
}

void PhysicsEntity::Update(GraphicsController* graphics)
{
	graphics->GetEntityModel(m_modelID)->SetLocation(m_locationX, m_locationY, m_locationZ);
	graphics->GetEntityModel(m_modelID)->SetRotation(m_rotationX, m_rotationY, m_rotationZ);
}

// DEAL WITH THIS LATER?
void PhysicsEntity::GetBoundingBox(D3DXVECTOR3& a, D3DXVECTOR3& b, D3DXVECTOR3& c, D3DXVECTOR3& d)
{
	// Code reuse :(
	D3DXMATRIX rotationMatrix;
	float yaw, pitch, roll;

	yaw = m_rotationY * 0.0174532925f;
	pitch = m_rotationX * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, 0.0f, 0.0f);

	a.x = -m_width / 2.0f;
	a.y = 0.0f;
	a.z = m_depth / 2.0f;

	b.x = m_width / 2.0f;
	b.y = 0.0f;
	b.z = m_depth / 2.0f;

	c.x = m_width / 2.0f;
	c.y = 0.0f;
	c.z = -m_depth / 2.0f;

	d.x = -m_width / 2.0f;
	d.y = 0.0f;
	d.z = -m_depth / 2.0f;

	D3DXVec3TransformCoord(&a, &a, &rotationMatrix);
	D3DXVec3TransformCoord(&b, &b, &rotationMatrix);
	D3DXVec3TransformCoord(&c, &c, &rotationMatrix);
	D3DXVec3TransformCoord(&d, &d, &rotationMatrix);

	a.x += m_locationX;
	a.y += m_locationY;
	a.z += m_locationZ;
	b.x += m_locationX;
	b.y += m_locationY;
	b.z += m_locationZ;
	c.x += m_locationX;
	c.y += m_locationY;
	c.z += m_locationZ;
	d.x += m_locationX;
	d.y += m_locationY;
	d.z += m_locationZ;

	return;
}