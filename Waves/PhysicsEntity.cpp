#include "PhysicsEntity.h"


PhysicsEntity::PhysicsEntity() : Entity()
{
	m_modelID = 0;

	m_velocityX = 0.0f;
	m_velocityY = 0.0f;
	m_velocityZ = 0.0f;

	m_velocityDamping = 0.1f;
	m_maxVelocity = 2.0f;
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
	Entity::Update();

	if (m_impulseX != 0.0f)
	{
		m_velocityX += m_impulseX;
		m_impulseX = 0.0f;
	}
	if (m_impulseY != 0.0f)
	{
		m_velocityY += m_impulseY;
		m_impulseY = 0.0f;
	}
	if (m_impulseZ != 0.0f)
	{
		m_velocityZ += m_impulseZ;
		m_impulseZ = 0.0f;
	}

	if (abs(m_velocityX) > m_maxVelocity) m_velocityX = m_maxVelocity*(m_velocityX / abs(m_velocityX));
	if (abs(m_velocityY) > m_maxVelocity) m_velocityY = m_maxVelocity*(m_velocityY / abs(m_velocityY));
	if (abs(m_velocityZ) > m_maxVelocity) m_velocityZ = m_maxVelocity*(m_velocityZ / abs(m_velocityZ));

	m_velocityX -= m_velocityX*m_velocityDamping;
	m_velocityY -= m_velocityY*m_velocityDamping;
	m_velocityZ -= m_velocityZ*m_velocityDamping;

	ApplyTranslationRelative(m_velocityX, m_velocityY, m_velocityZ); 

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

void PhysicsEntity::OrientToTerrain(ProceduralTerrain* terrain, float time)
{
	D3DXVECTOR3 a, b, c, d;
	float fr, fl, br, bl;
	float roll, pitch;

	roll = 0; pitch = 0;

	float x, y, z;
	GetLocation(x, y, z);
	GetBoundingBox(a, b, c, d);

	fr = terrain->CalculateDeterministicHeight(b.x, b.z, time);
	fl = terrain->CalculateDeterministicHeight(a.x, a.z, time);
	bl = terrain->CalculateDeterministicHeight(d.x, d.z, time);
	br = terrain->CalculateDeterministicHeight(c.x, c.z, time);

	roll = -180.0f / 3.14f*atan2(((fl + bl) - (fr + br)) / 2.0, 3); // fix absolue 3 as width
	SetRotation(NULL, NULL, roll);

	pitch = -180.0f / 3.14f*atan2(((fl + fr) - (bl + br)) / 2.0, 5);
	SetRotation(pitch, NULL, NULL);


	SetLocation(NULL, terrain->CalculateDeterministicHeight(x, z, time) + 1, NULL);

	return;
}

void PhysicsEntity::ApplyImpulse(float x, float y, float z)
{
	if (x) m_impulseX += x;
	if (y) m_impulseY += y;
	if (z) m_impulseZ += z;
}