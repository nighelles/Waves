#pragma once

#include "EntityModel.h"
#include <d3dx10math.h>

#include <fstream>

#include "PhysicsDefs.h"

#define MAPSIZE 100

#define VERTEX(x,z) (x+z*MAPSIZE)
#define INDEX(a) (a+1)

class Terrain : public EntityModel
{
public:
	Terrain();
	~Terrain();

	virtual bool InitializeBuffers(ID3D11Device* device);
	virtual bool Initialize(ID3D11Device* device, char* textureFilename);

	virtual bool LoadModel();

	virtual bool LoadTerrainMap(char* filename);
	virtual bool SaveTerrainMap(char* filename);

	virtual void Update();

	virtual float CalculateDeterministicHeight(float x, float y, float t);
	virtual float GetNormalForLocation(float x, float y);

	virtual void ApplyVerticalOffset(int xLoc, int zLoc, float radius, float height);
	virtual void SetVerticalOffset(int xLoc, int zLoc, float radius, float height);
	void SmoothTerrainMap(int xLoc, int zLoc, float radius);

protected:
	virtual void regenerateModelFromTerrainMap();
	virtual void regenerateNormals();
	void RenderBuffers(ID3D11DeviceContext*);
	float terrainMap[MAPSIZE][MAPSIZE];

	float m_gridSize;
	float m_initialHeight;

	bool m_modelUpdateNeeded;
};

