#include "Light.h"


Light::Light()
{
}


Light::~Light()
{
}

void Light::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = D3DXVECTOR4(red, green, blue, alpha);
	return;
}


void Light::SetDirection(float x, float y, float z)
{
	m_direction = D3DXVECTOR3(x, y, z);
	return;
}


D3DXVECTOR4 Light::GetDiffuseColor()
{
	return m_diffuseColor;
}


D3DXVECTOR3 Light::GetDirection()
{
	return m_direction;
}