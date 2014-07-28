#pragma once

#include <d3d11.h>
#include <d3dx10math.h>

#include "EntityModel.h"
#include "GraphicsController.h"

class Entity
{
public:
	Entity();
	~Entity();

	void Update();

	bool Initialize();
	void Shutdown();

	bool InitializeModel(GraphicsController* graphics, char* modelFilename, WCHAR* textureFilename);

protected:
	float m_locationX;
	float m_locationY;
	float m_locationZ;

	float m_rotationX;
	float m_rotationY;
	float m_rotationZ;

	int m_modelID;
};

