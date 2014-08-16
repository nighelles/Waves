#pragma once

#include "Entity.h"
#include "EntityModel.h"
#include "GraphicsController.h"

class PhysicsEntity : public Entity
{
public:
	PhysicsEntity();
	~PhysicsEntity();

	void Shutdown();

	bool InitializeModel(GraphicsController* graphics, char* modelFilename, WCHAR* textureFilename);
	
	void Tick(float dt);
	virtual void Render(GraphicsController* graphics);
	void OrientToTerrain(ProceduralTerrain* terrain, float time);

	void GetBoundingBox(D3DXVECTOR3&, D3DXVECTOR3&, D3DXVECTOR3&, D3DXVECTOR3&);
	
	void ApplyImpulseDirect(float x, float y, float z);
	void ApplyImpulseAccel (float x, float y, float z, float dt);

	void SetVelocity(float x, float y, float z);

	D3DXVECTOR3 GetVelocity() { return D3DXVECTOR3(m_velocityX, m_velocityY, m_velocityZ); };

	float m_impulseX, m_impulseY, m_impulseZ;
	float m_velocityX, m_velocityY, m_velocityZ;

	bool m_grounded;
	bool m_underwater;

	float Bouyancy() const { return m_bouyancy; }
	void Bouyancy(float val) { m_bouyancy = val; }

	int ModelID() const { return m_modelID; }
	void ModelID(int val) { m_modelID = val; }

	// accessors
	EntityModel* GetEntityModel() const { return m_entityModel; }
	void SetEntityModel(EntityModel* val) { m_entityModel = val; }

protected:
	EntityModel* m_entityModel;

	int m_modelID;
	float m_velocityDamping;

	float m_bouyancy;
};

