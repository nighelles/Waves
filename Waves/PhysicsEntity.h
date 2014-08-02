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

	void GetBoundingBox(D3DXVECTOR3&, D3DXVECTOR3&, D3DXVECTOR3&, D3DXVECTOR3&);
	
protected:
	int m_modelID;
};

