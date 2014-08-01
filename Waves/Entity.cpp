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

	m_modelID = 0;
}


Entity::~Entity()
{
}

bool Entity::Initialize() {
	return true;
}

bool Entity::InitializeModel(GraphicsController* graphics, char* modelFilename, WCHAR* textureFilename)
{
	bool result;

	result = graphics->InitializeEntityModel(modelFilename, textureFilename);
	if (result == -1)
		return false;
	else
		m_modelID = result;

	return true;
}

void Entity::Shutdown()
{

}

void Entity::Update(GraphicsController* graphics)
{
	graphics->GetEntityModel(m_modelID)->SetLocation(m_locationX, m_locationY, m_locationZ);
	graphics->GetEntityModel(m_modelID)->SetRotation(m_rotationX, m_rotationY, m_rotationZ);
}

void Entity::GetCameraLocation(float& x, float& y, float& z)
{
	D3DXVECTOR3 cameraLocation;
	D3DXMATRIX rotationMatrix;

	//code reuse :(
	float yaw, pitch, roll;
	yaw = m_rotationY * 0.0174532925f;
	pitch = m_rotationX * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f; 
	
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);

	cameraLocation.x = 0.0f;
	cameraLocation.y = 60.0f; // Default is a hover camera
	cameraLocation.z = -10.0f;

	D3DXVec3TransformCoord(&cameraLocation, &cameraLocation, &rotationMatrix);

	x = m_locationX + cameraLocation.x;
	y = m_locationY + cameraLocation.y;
	z = m_locationZ + cameraLocation.z;
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

// DEAL WITH THIS LATER?
void Entity::GetBoundingBox(D3DXVECTOR3& a, D3DXVECTOR3& b, D3DXVECTOR3& c, D3DXVECTOR3& d)
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

void Entity::GetLocation(float& x, float& y, float& z)
{
	x = m_locationX;
	y = m_locationY;
	z = m_locationZ;
}