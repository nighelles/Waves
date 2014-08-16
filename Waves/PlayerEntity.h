#pragma once

#include "PhysicsEntity.h"

class PlayerEntity : public PhysicsEntity
{
public:
	PlayerEntity();
	~PlayerEntity();

	virtual void GetBindLocation(float&, float&, float&); // for weapons or items

	void Movement(float x, float y, float z, float dt);
	void Stop(float dt);
	void Jump(float dt);

	bool Run() const { return m_run; }
	void Run(bool val) { m_run = val; }

	virtual void Render(GraphicsController* graphics);
protected:
	float m_maxWalkSpeed;
	float m_runMultiplier;
	float m_jumpStrength;
	float m_airbornControl;
	
	bool m_run;
};

