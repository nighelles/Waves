#pragma once

#include "Entity.h"

#define TORADIANS 0.0174532925f;

class Camera : public Entity
{
public:
	Camera();
	Camera(const Camera&);
	~Camera();

	void ApplyRotation(float x, float y, float z);
	void ApplyTranslation(float x, float y, float z);

	D3DXVECTOR3 GetPosition();
	D3DXVECTOR3 GetRotation();

	void Render();
	void GetViewMatrix(D3DXMATRIX&);

private:
	D3DXMATRIX m_viewMatrix;
};

