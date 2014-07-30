#pragma once

#include <d3dx10math.h>

class Light
{
public:
	Light();
	~Light();

	void SetDiffuseColor(float, float, float, float);
	void SetFillColor(float, float, float, float);
	void SetDirection(float, float, float);

	D3DXVECTOR4 GetDiffuseColor();
	D3DXVECTOR4 GetFillColor();
	D3DXVECTOR3 GetDirection();

private:
	D3DXVECTOR4 m_diffuseColor;
	D3DXVECTOR4 m_fillColor;
	D3DXVECTOR3 m_direction;
};

