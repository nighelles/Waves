#include "Terrain.h"


Terrain::Terrain() : EntityModel()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;
	m_model = 0;

	m_gridSize = 4.0f;

	m_initialHeight = 5.0f;
}


Terrain::~Terrain()
{
}

bool Terrain::InitializeBuffers(ID3D11Device* device)
{
	Vertex* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i;

	m_device = device;

	vertices = new Vertex[m_vertexCount];
	if (!vertices) return false;

	indices = new unsigned long[m_indexCount];
	if (!indices) return false;

	for (i = 0; i != m_vertexCount; ++i)
	{
		vertices[i].position = D3DXVECTOR3(m_model[i].x, m_model[i].y, m_model[i].z);
		vertices[i].texture = D3DXVECTOR2(m_model[i].tu, m_model[i].tv);
		vertices[i].normal = D3DXVECTOR3(m_model[i].nx, m_model[i].ny, m_model[i].nz);
		indices[i] = i;
	}

	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result)) return false;

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result)) return false;

	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;

	return true;
}

void Terrain::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	Vertex* vertices;
	unsigned long* indices;
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(Vertex);
	offset = 0;

	if (m_modelUpdateNeeded)
	{
		m_modelUpdateNeeded = false;
		vertices = new Vertex[m_vertexCount];

		for (int i = 0; i != m_vertexCount; ++i)
		{
			vertices[i].position = D3DXVECTOR3(m_model[i].x, m_model[i].y, m_model[i].z);
			vertices[i].texture = D3DXVECTOR2(m_model[i].tu, m_model[i].tv);
			vertices[i].normal = D3DXVECTOR3(m_model[i].nx, m_model[i].ny, m_model[i].nz);
		}

		D3D11_MAPPED_SUBRESOURCE resource;
		deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
		memcpy(resource.pData, &vertices[0], sizeof(Vertex)*m_vertexCount);
		deviceContext->Unmap(m_vertexBuffer, 0);

		delete[] vertices;

	}

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool Terrain::Initialize(ID3D11Device* device, WCHAR* textureFilename)
{
	bool result;

	LoadModel();

	result = InitializeBuffers(device);
	if (!result) return false;

	result = LoadTexture(device, textureFilename);
	if (!result) return false;

	return true;
}

bool Terrain::LoadModel()
{
	m_vertexCount = MAPSIZE*MAPSIZE*6;
	m_indexCount = m_vertexCount;

	m_model = new Model[m_indexCount];

	for (int x = 0; x != MAPSIZE; ++x) // generate initial map heights
	{
		for (int z = 0; z != MAPSIZE; ++z)
		{
			terrainMap[x][z] = 0.0;
		}
	}

	regenerateModelFromTerrainMap();

	return true;
}

void Terrain::Update()
{

}

void Terrain::regenerateModelFromTerrainMap()
{
	m_modelUpdateNeeded = true;

	int index = 0;

	for (int x = 0; x != MAPSIZE - 1; ++x)
	{
		for (int z = 0; z != MAPSIZE - 1; ++z)
		{
			m_model[index].x = x * m_gridSize - MAPSIZE*m_gridSize*0.5f;
			m_model[index].y = terrainMap[x][z] + m_initialHeight;
			m_model[index].z = z * m_gridSize - MAPSIZE*m_gridSize*0.5f;
			m_model[index].tu = 0.0f;
			m_model[index].tv = 0.0f;

			m_model[index + 1].x = (x + 1) * m_gridSize - MAPSIZE*m_gridSize*0.5f;
			m_model[index + 1].y = terrainMap[x + 1][z + 1] + m_initialHeight;
			m_model[index + 1].z = (z + 1) * m_gridSize - MAPSIZE*m_gridSize*0.5f;
			m_model[index + 1].tu = 1.0f;
			m_model[index + 1].tv = 1.0f;

			m_model[index + 2].x = (x + 1) * m_gridSize - MAPSIZE*m_gridSize*0.5f;
			m_model[index + 2].y = terrainMap[x + 1][z] + m_initialHeight;
			m_model[index + 2].z = z * m_gridSize - MAPSIZE*m_gridSize*0.5f;
			m_model[index + 2].tu = 1.0f;
			m_model[index + 2].tv = 0.0f;

			m_model[index + 3].x = x * m_gridSize - MAPSIZE*m_gridSize*0.5f;
			m_model[index + 3].y = terrainMap[x][z] + m_initialHeight;
			m_model[index + 3].z = z * m_gridSize - MAPSIZE*m_gridSize*0.5f;
			m_model[index + 3].tu = 0.0f;
			m_model[index + 3].tv = 0.0f;

			m_model[index + 4].x = x * m_gridSize - MAPSIZE*m_gridSize*0.5f;
			m_model[index + 4].y = terrainMap[x][z + 1] + m_initialHeight;
			m_model[index + 4].z = (z + 1) * m_gridSize - MAPSIZE*m_gridSize*0.5f;
			m_model[index + 4].tu = 0.0f;
			m_model[index + 4].tv = 1.0f;

			m_model[index + 5].x = (x + 1) * m_gridSize - MAPSIZE*m_gridSize*0.5f;
			m_model[index + 5].y = terrainMap[x + 1][z + 1] + m_initialHeight;
			m_model[index + 5].z = (z + 1) * m_gridSize - MAPSIZE*m_gridSize*0.5f;
			m_model[index + 5].tu = 1.0f;
			m_model[index + 5].tv = 1.0f;

			index += 6;
		}
	}
	regenerateNormals();

	return;
}

void Terrain::regenerateNormals()
{
	// Regenerate normals
	D3DXVECTOR3 vt1, vt2, vt3;
	D3DXVECTOR3 edge1, edge2;
	D3DXVECTOR3 normal;

	D3DXVECTOR3 normalMap[MAPSIZE*2][MAPSIZE];

	int flip = 0;

	for (int i = 0; i != m_vertexCount; i += 3)
	{
		vt1.x = m_model[i + 0].x; vt1.y = m_model[i + 0].y; vt1.z = m_model[i + 0].z;
		vt2.x = m_model[i + 1].x; vt2.y = m_model[i + 1].y; vt2.z = m_model[i + 1].z;
		vt3.x = m_model[i + 2].x; vt3.y = m_model[i + 2].y; vt3.z = m_model[i + 2].z;

		D3DXVec3Subtract(&edge1, &vt3, &vt1);
		D3DXVec3Subtract(&edge2, &vt2, &vt1);

		D3DXVec3Cross(&normal, &edge2, &edge1);

		D3DXVec3Normalize(&normal,&normal);

		m_model[i + 0].nx = normal.x; m_model[i + 0].ny = normal.y; m_model[i + 0].nz = normal.z;
		m_model[i + 1].nx = normal.x; m_model[i + 1].ny = normal.y; m_model[i + 1].nz = normal.z;
		m_model[i + 2].nx = normal.x; m_model[i + 2].ny = normal.y; m_model[i + 2].nz = normal.z;

	}

	// Then calculate vertex normals

	
	return;
}

float Terrain::CalculateDeterministicHeight(float x, float y, float t)
{
	int indexX, indexY;
	float avgZ;
	float weightX;
	float weightY;

	indexX = x / m_gridSize;
	indexY = y / m_gridSize;

	weightX = (x - indexX);
	weightY = (y - indexY);


	avgZ = ((1-weightX)*(1-weightY)*terrainMap[indexX][indexY] + 
			(1-weightX)*(weightY)*terrainMap[indexX][indexY+1] +
			(weightX)*(weightY)*terrainMap[indexX + 1][indexY + 1] +
			(weightX)*(1-weightY)*terrainMap[indexX+1][indexY]) / 4.0f;

	return avgZ;
}

void Terrain::ApplyVerticalOffset(int xLoc, int zLoc, float radius, float height)
{
	float dist;
	float worldX;
	float worldZ;

	for (int x = 0; x != MAPSIZE; ++x)
	{
		for (int z = 0; z != MAPSIZE; ++z)
		{
			worldX = x*m_gridSize - MAPSIZE*m_gridSize / 2.0;
			worldZ = z*m_gridSize - MAPSIZE*m_gridSize / 2.0;
			dist = sqrt(pow(xLoc-worldX,2) + pow(zLoc-worldZ,2));
			if (dist < 2) dist = 2;
			if (dist < radius)
			{
				terrainMap[x][z] += height / dist;
			}
		}
	}
	SmoothTerrainMap(xLoc, zLoc, radius);
	regenerateModelFromTerrainMap();
}

void Terrain::SetVerticalOffset(int xLoc, int zLoc, float radius, float offset)
{
	float dist;
	float worldX;
	float worldZ;

	for (int x = 0; x != MAPSIZE; ++x)
	{
		for (int z = 0; z != MAPSIZE; ++z)
		{
			worldX = x*m_gridSize - MAPSIZE*m_gridSize / 2.0;
			worldZ = z*m_gridSize - MAPSIZE*m_gridSize / 2.0;
			dist = sqrt(pow(xLoc - worldX, 2) + pow(zLoc - worldZ, 2));
			if (dist < 1) dist = 1;
			if (dist < radius)
			{
				terrainMap[x][z] = offset;
			}
		}
	}
	regenerateModelFromTerrainMap();
}

void Terrain::SmoothTerrainMap(int xLoc, int zLoc, float radius)
{
	float worldX, worldZ, dist;

	for (int x = 0; x != MAPSIZE; ++x)
	{
		for (int z = 0; z != MAPSIZE; ++z)
		{
			if (x > 0 && x < MAPSIZE && z > 0 && z < MAPSIZE)
			{
				worldX = x*m_gridSize - MAPSIZE*m_gridSize / 2.0;
				worldZ = z*m_gridSize - MAPSIZE*m_gridSize / 2.0;
				dist = sqrt(pow(xLoc - worldX, 2) + pow(zLoc - worldZ, 2));
				if (dist < radius)
				{
					terrainMap[x][z] = (terrainMap[x][z]*4 + terrainMap[x - 1][z - 1] + terrainMap[x + 1][z - 1] + terrainMap[x - 1][z + 1] + terrainMap[x + 1][z + 1]) / 8;
				}
			}
		}
	}
}

bool Terrain::LoadTerrainMap(char* filename)
{
	ifstream fin;

	fin.open(filename);

	if (fin.fail())
	{
		OutputDebugString(L"Failed to open output file.\n");
		return false;
	}

	for (int x = 0; x != MAPSIZE; ++x)
	{
		for (int z = 0; z != MAPSIZE; ++z)
		{
			fin >> terrainMap[x][z];
		}
	}

	fin.close();

	regenerateModelFromTerrainMap();

	return true;
}

bool Terrain::SaveTerrainMap(char* filename)
{
	ofstream fout;

	fout.open(filename);

	if (fout.fail())
	{
		OutputDebugString(L"Failed to open output file.\n");
		return false;
	}

	for (int x = 0; x != MAPSIZE; ++x)
	{
		for (int z = 0; z != MAPSIZE; ++z)
		{
			fout << terrainMap[x][z] << " ";
		}
	}

	fout.close();

	return true;
}

