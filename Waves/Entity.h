#pragma once

#include <d3d11.h>
#include <d3dx10math.h>

#include "EntityModel.h"

class Entity
{
public:
	Entity();
	~Entity();

	void Update();

	void Shutdown();

	bool Initialize();
	bool InitializeModel(ID3D11Device* device, WCHAR* textureFilename);
	EntityModel* GetModel();

	void Render(ID3D11DeviceContext*);

protected:
	EntityModel* m_Model;

	float m_locationX;
	float m_locationY;
	float m_locationZ;

	float m_rotationX;
	float m_rotationY;
	float m_rotationZ;
};

