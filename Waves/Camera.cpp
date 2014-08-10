#include "Camera.h"


Camera::Camera() : Entity()
{
	D3DXVECTOR3 up, position, lookAt;
	float yaw, pitch, roll;
	D3DXMATRIX rotationMatrix;

	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;
	position.x = 0;
	position.y = 0;
	position.z = -1;

	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	lookAt = position + lookAt;

	D3DXMatrixLookAtLH(&m_hudViewMatrix, &position, &lookAt, &up);
}

Camera::Camera(const Camera& other)
{
}

Camera::~Camera()
{
}

void Camera::ApplyRotation(float x, float y, float z)
{
	m_rotationX += x;
	m_rotationY += y;
	m_rotationZ += z;

	if (m_rotationX > 90) m_rotationX = 90; if (m_rotationX < -90) m_rotationX = -90;
	if (m_rotationY > 360) m_rotationY -= 360; if (m_rotationY < -360) m_rotationY += 360;
	if (m_rotationZ > 360) m_rotationZ -= 360; if (m_rotationZ < -360) m_rotationZ += 360;
}

void Camera::ApplyTranslation(float x, float y, float z)
{
	D3DXVECTOR3 translation;
	D3DXMATRIX rotationMatrix;
	float yaw;
	yaw = m_rotationY * 0.0174532925f;
	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, 0, 0);

	translation.x = x; translation.y = y; translation.z = z;

	D3DXVec3TransformCoord(&translation, &translation, &rotationMatrix);
	
	m_locationX += translation.x; 
	m_locationY += translation.y; 
	m_locationZ += translation.z;
}
void Camera::Render()
{
	D3DXVECTOR3 up, position, lookAt;
	float yaw, pitch, roll;
	D3DXMATRIX rotationMatrix;

	up.x = 0.0f; 
	up.y = 1.0f; 
	up.z = 0.0f;
	position.x = m_locationX; 
	position.y = m_locationY; 
	position.z = m_locationZ;
	
	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);

	D3DXVec3TransformCoord(&up, &up, &rotationMatrix);
	lookAt = position + lookAt;

	D3DXMatrixLookAtLH(&m_viewMatrix, &position, &lookAt, &up);

	return;
}

void Camera::GetViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}

void Camera::GetHUDViewMatrix(D3DXMATRIX& viewMatrix)
{
	viewMatrix = m_hudViewMatrix;
	return;
}

void Camera::Update()
{
	if (m_binded)
	{
		float x, y, z;
		m_bindedEntity->GetCameraLocation(x, y, z);
		SetLocation(x, y, z);
	}
	return;
}

D3DXVECTOR3 Camera::GetPosition()
{
	return D3DXVECTOR3(m_locationX, m_locationY, m_locationZ);
}

D3DXVECTOR3 Camera::GetDirection()
{
	D3DXVECTOR3 up, position, lookAt;
	float yaw, pitch, roll;
	D3DXMATRIX rotationMatrix;

	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;
	position.x = m_locationX;
	position.y = m_locationY;
	position.z = m_locationZ;

	lookAt.x = 0.0f;
	lookAt.y = 0.0f;
	lookAt.z = 1.0f;

	pitch = m_rotationX * 0.0174532925f;
	yaw = m_rotationY * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, pitch, roll);
	D3DXVec3TransformCoord(&lookAt, &lookAt, &rotationMatrix);

	return lookAt;
}

D3DXVECTOR3 Camera::GetRotation()
{
	return D3DXVECTOR3(m_rotationX, m_rotationY, m_rotationZ);
}


