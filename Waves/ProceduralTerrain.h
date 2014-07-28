#pragma once
#include "EntityModel.h"

class ProceduralTerrain : public EntityModel
{
public:
	ProceduralTerrain();
	~ProceduralTerrain();

	virtual bool Initialize(ID3D11Device* device, WCHAR* textureFilename);
	virtual bool LoadModel();

	virtual void regenerateModelFromTerrainMap();

protected:
	int terrainMap[100][100];
};

