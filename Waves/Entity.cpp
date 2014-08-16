#include "Entity.h"


Entity::Entity()
{
	m_locationX = 0.0f;
	m_locationY = 0.0f;
	m_locationZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;

	m_cameraX = 0.0f;
	m_cameraY = 0.0f;
	m_cameraZ = 0.0f;

	m_width = 3.0f;
	m_height = 1.0f;
	m_depth = 5.0f;

	m_binded = false;
	m_bindedEntity = 0;
}


Entity::~Entity()
{
}

bool Entity::Initialize() {
	return true;
}


void Entity::Shutdown()
{
// 	if (m_bindedEntity)
// 	{
// 		m_bindedEntity->Shutdown();
// 		delete m_bindedEntity;
// 		m_bindedEntity = 0;
// 	}
}


//These two functions need to be modified to load this information from a file.

void Entity::GetCameraLocation(float& x, float& y, float& z)
{
	D3DXVECTOR3 cameraLocation;
	D3DXMATRIX rotationMatrix;

	//code reuse :(
	float yaw, pitch, roll;
	yaw = m_rotationY * 0.0174532925f;
	pitch = 0;// m_rotationX * 0.0174532925f;
	roll = 0;// m_rotationZ * 0.0174532925f;
	
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	cameraLocation.x = 0.0f;
	cameraLocation.y = 0.9f; // Default is a hover camera
	cameraLocation.z = 0.0f;

	D3DXVec3TransformCoord(&cameraLocation, &cameraLocation, &rotationMatrix);

	x = m_locationX + cameraLocation.x;
	y = m_locationY + cameraLocation.y;
	z = m_locationZ + cameraLocation.z;
	return;
}

void Entity::GetBindLocation(float& x, float& y, float& z)
{
	GetCameraLocation(x, y, z);
	return;
}

void Entity::ApplyRotation(float x, float y, float z)
{
	m_rotationX += x;
	m_rotationY += y;
	m_rotationZ += z;

	if (m_rotationX > 360) m_rotationX -= 360; if (m_rotationX < -360) m_rotationX += 360;
	if (m_rotationY > 360) m_rotationY -= 360; if (m_rotationY < -360) m_rotationY += 360;
	if (m_rotationZ > 360) m_rotationZ -= 360; if (m_rotationZ < -360) m_rotationZ += 360;
}

void Entity::SetRotation(float x, float y, float z)
{
	if (x) m_rotationX = x;
	if (y) m_rotationY = y;
	if (z) m_rotationZ = z;
}

void Entity::SetLocation(float x, float y, float z)
{
	if (x) m_locationX = x;
	if (y) m_locationY = y;
	if (z) m_locationZ = z;
}

void Entity::ApplyTranslation(float x, float y, float z)
{
	m_locationX += x;
	m_locationY += y;
	m_locationZ += z;
}

void Entity::ApplyTranslationRelative(float x, float y, float z)
{
	D3DXVECTOR3 translation;
	D3DXMATRIX rotationMatrix;
	float yaw, pitch, roll;

	yaw = m_rotationY * 0.0174532925f;
	pitch = m_rotationX * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	//only allow rotation around y, simplify things a bit at first
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	translation.x = x; translation.y = y; translation.z = z;

	D3DXVec3TransformCoord(&translation, &translation, &rotationMatrix);

	m_locationX += translation.x;
	m_locationY += translation.y;
	m_locationZ += translation.z;
}

void Entity::GetLocation(float& x, float& y, float& z)
{
	x = m_locationX;
	y = m_locationY;
	z = m_locationZ;
}

void Entity::GetRotation(float& x, float& y, float& z)
{
	x = m_rotationX;
	y = m_rotationY;
	z = m_rotationZ;
}

void Entity::BindToEntity(Entity* entity)
{
	m_bindedEntity = entity;
	m_binded = true;

	return;
}

void Entity::UnbindFromEntity()
{
	m_binded = false;

	return;
}

void Entity::Update()
{
	if (m_binded)
	{
		float x, y, z;
		m_bindedEntity->GetBindLocation(x, y, z);
		SetLocation(x, y, z);
		m_bindedEntity->GetRotation(x, y, z);
		SetRotation(x, y, z);
	}
	return;
}