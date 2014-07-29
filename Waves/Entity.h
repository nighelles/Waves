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

	void Update(GraphicsController* graphics);

	bool Initialize();
	void Shutdown();

	bool InitializeModel(GraphicsController* graphics, char* modelFilename, WCHAR* textureFilename);

	void GetCameraLocation(float&, float&, float&);
	void ApplyRotation(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetLocation(float x, float y, float z);
	void ApplyTranslation(float x, float y, float z);
	void ApplyTranslationRelative(float x, float y, float z);

	void GetLocation(float&, float&, float&);
	void GetBoundingBox(D3DXVECTOR3&, D3DXVECTOR3&, D3DXVECTOR3&, D3DXVECTOR3&);

protected:
	float m_locationX;
	float m_locationY;
	float m_locationZ;

	float m_rotationX;
	float m_rotationY;
	float m_rotationZ;

	float m_cameraX;
	float m_cameraY;
	float m_cameraZ;

	float m_width;
	float m_height;
	float m_depth;

	int m_modelID;
};

