#include "Camera.h"


Camera::Camera()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;
	
	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}

Camera::Camera(const Camera& other)
{
}

Camera::~Camera()
{
}

void Camera::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}

void Camera::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
}

D3DXVECTOR3 Camera::GetPosition()
{
	return D3DXVECTOR3(m_positionX, m_positionY, m_positionZ);
}

D3DXVECTOR3 Camera::GetRotation()
{
	return D3DXVECTOR3(m_rotationX, m_rotationY, m_rotationZ);
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
	
	m_positionX += translation.x; 
	m_positionY += translation.y; 
	m_positionZ += translation.z;
}
void Camera::Render()
{
	D3DXVECTOR3 up, position, lookAt;
	float yaw, pitch, roll;
	D3DXMATRIX rotationMatrix;

	up.x = 0.0f; 
	up.y = 1.0f; 
	up.z = 0.0f;
	position.x = m_positionX; 
	position.y = m_positionY; 
	position.z = m_positionZ;
	
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