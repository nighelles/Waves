#pragma once

#include "Entity.h"
#include "EntityModel.h"
#include "GraphicsController.h"

class PhysicsEntity : public Entity
{
public:
	PhysicsEntity();
	~PhysicsEntity();

	bool InitializeModel(GraphicsController* graphics, char* modelFilename, WCHAR* textureFilename);
	
	void Update(GraphicsController* graphics);
	void OrientToTerrain(ProceduralTerrain* terrain, float time);

	void GetBoundingBox(D3DXVECTOR3&, D3DXVECTOR3&, D3DXVECTOR3&, D3DXVECTOR3&);
	
	void ApplyImpulse(float x, float y, float z);

	D3DXVECTOR3 GetVelocity() { return D3DXVECTOR3(m_velocityX, m_velocityY, m_velocityZ); };


protected:
	int m_modelID;

	float m_impulseX, m_impulseY, m_impulseZ;
	float m_velocityX, m_velocityY, m_velocityZ;

	float m_velocityDamping;
	float m_maxVelocity;
};

