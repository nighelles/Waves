#include "ProceduralTerrain.h"


ProceduralTerrain::ProceduralTerrain()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
	m_model = 0;
}


ProceduralTerrain::~ProceduralTerrain()
{
}


bool ProceduralTerrain::Initialize(ID3D11Device* device, WCHAR* textureFilename)
{
	bool result;

	LoadModel();

	result = InitializeBuffers(device);
	if (!result) return false;

	result = LoadTexture(device, textureFilename);
	if (!result) return false;

	return true;
}

bool ProceduralTerrain::LoadModel()
{
	m_vertexCount = 60000;
	m_indexCount = m_vertexCount;

	m_model = new Model[m_indexCount];

	for (int x = 0; x != 100; ++x) // generate initial map heights
	{
		for (int z = 0; z != 100; ++z)
		{
			terrainMap[x][z] = 0;
		}
	}

	regenerateModelFromTerrainMap();

	return true;
}

void ProceduralTerrain::regenerateModelFromTerrainMap() 
{
	int index = 0;

	for (int x = 0; x != 100; ++x)
	{
		for (int z = 0; z != 100; ++z)
		{
			m_model[index].x = x-50;
			m_model[index].y = terrainMap[x][z];
			m_model[index].z = z-50;
			m_model[index].tu = 0.0f;
			m_model[index].tv = 0.0f;
			m_model[index].nx = 0.0f;
			m_model[index].ny = 1.0f;
			m_model[index].nz = 0.0f;

			m_model[index + 1].x = x + 1 - 50;
			m_model[index+1].y = terrainMap[x+1][z+1];
			m_model[index + 1].z = z + 1 - 50;
			m_model[index+1].tu = 1.0f;
			m_model[index+1].tv = 1.0f;
			m_model[index+1].nx = 0.0f;
			m_model[index+1].ny = 1.0f;
			m_model[index+1].nz = 0.0f;

			m_model[index + 2].x = x + 1 - 50;
			m_model[index+2].y = terrainMap[x+1][z];
			m_model[index + 2].z = z - 50;
			m_model[index+2].tu = 1.0f;
			m_model[index+2].tv = 0.0f;
			m_model[index+2].nx = 0.0f;
			m_model[index+2].ny = 1.0f;
			m_model[index+2].nz = 0.0f;

			m_model[index + 3].x = x - 50;
			m_model[index+3].y = terrainMap[x][z];
			m_model[index + 3].z = z - 50;
			m_model[index+3].tu = 0.0f;
			m_model[index+3].tv = 0.0f;
			m_model[index+3].nx = 0.0f;
			m_model[index+3].ny = 1.0f;
			m_model[index+3].nz = 0.0f;

			m_model[index + 4].x = x - 50;
			m_model[index+4].y = terrainMap[x][z+1];
			m_model[index + 4].z = z + 1 - 50;
			m_model[index+4].tu = 0.0f;
			m_model[index+4].tv = 1.0f;
			m_model[index+4].nx = 0.0f;
			m_model[index+4].ny = 1.0f;
			m_model[index+4].nz = 0.0f;

			m_model[index + 5].x = x + 1 - 50;
			m_model[index+5].y = terrainMap[x+1][z+1];
			m_model[index + 5].z = z + 1 - 50;
			m_model[index+5].tu = 1.0f;
			m_model[index+5].tv = 1.0f;
			m_model[index+5].nx = 0.0f;
			m_model[index+5].ny = 1.0f;
			m_model[index+5].nz = 0.0f;

			index += 6;
		}
	}
}