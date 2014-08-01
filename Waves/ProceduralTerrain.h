#pragma once
#include "EntityModel.h"
#include <d3dx10math.h>

class ProceduralTerrain : public EntityModel
{
public:
	ProceduralTerrain();
	~ProceduralTerrain();

	virtual bool InitializeBuffers(ID3D11Device* device);

	virtual bool Initialize(ID3D11Device* device, WCHAR* textureFilename);
	virtual bool LoadModel();

	virtual void Update(float loopCompletion);
	
	virtual float CalculateDeterministicHeight(float x, float y, float t);

protected:
	virtual void regenerateModelFromTerrainMap();
	void RenderBuffers(ID3D11DeviceContext*);

	float terrainMap[100][100];
};

