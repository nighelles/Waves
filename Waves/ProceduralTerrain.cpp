#include "ProceduralTerrain.h"

ProceduralTerrain::ProceduralTerrain() : Terrain()
{
	m_initialHeight = 0.0f;
}


ProceduralTerrain::~ProceduralTerrain()
{
	
}


float ProceduralTerrain::CalculateDeterministicHeight(float x, float y, float t)
{
	// Rolling waves with a slower wave
	return 2.0*sin(6.28*x / 100.0 + 3.14 * 5 * t);// +3.0*sin(3.14*x / 100.0 + 2 * 3.14*t);
	//Rolling waves intersecting in x and y
	//return 5.0*sin(6.28*x / 100.0 + 3.14 * 5 * t) + 5.0*sin(6.28*y / 100.0 + 3.14 * 5 * t);
	//return 5.0f*sin(y/20.0f);
}

void ProceduralTerrain::Update(float loopCompletion)
{
	
}

