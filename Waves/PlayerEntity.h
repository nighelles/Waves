#pragma once

#include "PhysicsEntity.h"

class PlayerEntity : public PhysicsEntity
{
public:
	PlayerEntity();
	~PlayerEntity();

	void Movement(float x, float y, float z, float dt);
	void Stop(float dt);
	void Jump(float dt);

	bool Run() const { return m_run; }
	void Run(bool val) { m_run = val; }

protected:
	float m_maxWalkSpeed;
	float m_runMultiplier;
	float m_jumpStrength;
	float m_airbornControl;
	
	bool m_run;
};

