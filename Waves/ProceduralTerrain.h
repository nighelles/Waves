#pragma once
#include "Terrain.h"

class ProceduralTerrain : public Terrain
{
public:
	ProceduralTerrain();
	~ProceduralTerrain();

	virtual void Update(float loopCompletion);
	
	virtual float CalculateDeterministicHeight(float x, float y, float t);
};

