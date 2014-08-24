#include "Terrain.h"


Terrain::Terrain() : EntityModel()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;

	m_gridSize = METERS(4);

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
	int i, vIndex, tIndex, nIndex;

	m_device = device;

	vertices = new Vertex[m_vertexCount];
	if (!vertices) return false;

	indices = new unsigned long[m_indexCount];
	if (!indices) return false;

	for (i = 0; i != uniqueFaceCount * 3; ++i)
	{
		indices[i] = i;
	}

	for (int i = 0; i != uniqueFaceCount; ++i)
	{
		vIndex = m_modelDesc[i].vIndex1 - 1;
		tIndex = m_modelDesc[i].tIndex1 - 1;
		nIndex = m_modelDesc[i].nIndex1 - 1;

		vertices[3 * i].position = D3DXVECTOR3(uniqueVertices[vIndex].x, uniqueVertices[vIndex].y, uniqueVertices[vIndex].z);
		vertices[3 * i].texture = D3DXVECTOR2(uniqueTexcoords[tIndex].x, uniqueTexcoords[tIndex].y);
		vertices[3 * i].normal = D3DXVECTOR3(uniqueNormals[nIndex].x, uniqueNormals[nIndex].y, uniqueNormals[nIndex].z);

		vIndex = m_modelDesc[i].vIndex2 - 1;
		tIndex = m_modelDesc[i].tIndex2 - 1;
		nIndex = m_modelDesc[i].nIndex2 - 1;

		vertices[3 * i + 1].position = D3DXVECTOR3(uniqueVertices[vIndex].x, uniqueVertices[vIndex].y, uniqueVertices[vIndex].z);
		vertices[3 * i + 1].texture = D3DXVECTOR2(uniqueTexcoords[tIndex].x, uniqueTexcoords[tIndex].y);
		vertices[3 * i + 1].normal = D3DXVECTOR3(uniqueNormals[nIndex].x, uniqueNormals[nIndex].y, uniqueNormals[nIndex].z);

		vIndex = m_modelDesc[i].vIndex3 - 1;
		tIndex = m_modelDesc[i].tIndex3 - 1;
		nIndex = m_modelDesc[i].nIndex3 - 1;

		vertices[3 * i + 2].position = D3DXVECTOR3(uniqueVertices[vIndex].x, uniqueVertices[vIndex].y, uniqueVertices[vIndex].z);
		vertices[3 * i + 2].texture = D3DXVECTOR2(uniqueTexcoords[tIndex].x, uniqueTexcoords[tIndex].y);
		vertices[3 * i + 2].normal = D3DXVECTOR3(uniqueNormals[nIndex].x, uniqueNormals[nIndex].y, uniqueNormals[nIndex].z);

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
	int vIndex, tIndex, nIndex;

	stride = sizeof(Vertex);
	offset = 0;

	if (m_modelUpdateNeeded)
	{
		m_modelUpdateNeeded = false;
		vertices = new Vertex[m_vertexCount];

		for (int i = 0; i != uniqueFaceCount; ++i)
		{
			vIndex = m_modelDesc[i].vIndex1 - 1;
			tIndex = m_modelDesc[i].tIndex1 - 1;
			nIndex = m_modelDesc[i].nIndex1 - 1;

			vertices[3 * i].position = D3DXVECTOR3(uniqueVertices[vIndex].x, uniqueVertices[vIndex].y, uniqueVertices[vIndex].z);
			vertices[3 * i].texture = D3DXVECTOR2(uniqueTexcoords[tIndex].x, uniqueTexcoords[tIndex].y);
			vertices[3 * i].normal = D3DXVECTOR3(uniqueNormals[nIndex].x, uniqueNormals[nIndex].y, uniqueNormals[nIndex].z);

			vIndex = m_modelDesc[i].vIndex2 - 1;
			tIndex = m_modelDesc[i].tIndex2 - 1;
			nIndex = m_modelDesc[i].nIndex2 - 1;

			vertices[3 * i + 1].position = D3DXVECTOR3(uniqueVertices[vIndex].x, uniqueVertices[vIndex].y, uniqueVertices[vIndex].z);
			vertices[3 * i + 1].texture = D3DXVECTOR2(uniqueTexcoords[tIndex].x, uniqueTexcoords[tIndex].y);
			vertices[3 * i + 1].normal = D3DXVECTOR3(uniqueNormals[nIndex].x, uniqueNormals[nIndex].y, uniqueNormals[nIndex].z);

			vIndex = m_modelDesc[i].vIndex3 - 1;
			tIndex = m_modelDesc[i].tIndex3 - 1;
			nIndex = m_modelDesc[i].nIndex3 - 1;

			vertices[3 * i + 2].position = D3DXVECTOR3(uniqueVertices[vIndex].x, uniqueVertices[vIndex].y, uniqueVertices[vIndex].z);
			vertices[3 * i + 2].texture = D3DXVECTOR2(uniqueTexcoords[tIndex].x, uniqueTexcoords[tIndex].y);
			vertices[3 * i + 2].normal = D3DXVECTOR3(uniqueNormals[nIndex].x, uniqueNormals[nIndex].y, uniqueNormals[nIndex].z);

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

bool Terrain::Initialize(ID3D11Device* device, char* textureFilename)
{
	bool result;

	LoadModel();

	m_device = device;

	result = InitializeBuffers(device);
	if (!result) return false;

	result = loadBTWFile(textureFilename);
	if (!result) return false;

	return true;
}

bool Terrain::LoadModel()
{
	m_vertexCount = MAPSIZE*MAPSIZE*6;
	m_indexCount = m_vertexCount;

	for (int x = 0; x != MAPSIZE; ++x) // generate initial map heights
	{
		for (int z = 0; z != MAPSIZE; ++z)
		{
			terrainMap[x][z] = 5.0f;
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

	uniqueFaceCount = (MAPSIZE - 1)*(MAPSIZE - 1)*2;
	uniqueVertexCount = (MAPSIZE)*(MAPSIZE);
	uniqueNormalCount = uniqueVertexCount;
	uniqueTextureCount = 4;

	m_modelDesc = new UniqueFace[uniqueFaceCount];
	uniqueVertices = new UniqueVertex[uniqueVertexCount];
	uniqueNormals = new UniqueVertex[uniqueNormalCount];
	uniqueTexcoords = new UniqueVertex[uniqueTextureCount];

	uniqueTexcoords[0].x = 0;
	uniqueTexcoords[0].y = 0;

	uniqueTexcoords[1].x = 0;
	uniqueTexcoords[1].y = 1;

	uniqueTexcoords[2].x = 1;
	uniqueTexcoords[2].y = 1;

	uniqueTexcoords[3].x = 1;
	uniqueTexcoords[3].y = 0;

	for (int z = 0; z != MAPSIZE; ++z)
	{
		for (int x = 0; x != MAPSIZE; ++x)
		{
			uniqueVertices[VERTEX(x, z)].x = m_gridSize*x - m_gridSize*MAPSIZE / 2;
			uniqueVertices[VERTEX(x, z)].y = terrainMap[x][z];
			uniqueVertices[VERTEX(x, z)].z = m_gridSize*z - m_gridSize*MAPSIZE / 2;

			uniqueNormals[VERTEX(x, z)].x = 0;
			uniqueNormals[VERTEX(x, z)].y = 1;
			uniqueNormals[VERTEX(x, z)].z = 0;
		}
	}

	int curFace = 0;

	for (int z = 0; z != MAPSIZE-1; ++z)
	{
		for (int x = 0; x != MAPSIZE-1; ++x)
		{
			m_modelDesc[curFace].vIndex1 = (x)+MAPSIZE*(z)+1;
			m_modelDesc[curFace].vIndex2 = (x+1)+MAPSIZE*(z+1)+1;
			m_modelDesc[curFace].vIndex3 = (x+1)+MAPSIZE*(z)+1;

			m_modelDesc[curFace].nIndex1 = (x)+MAPSIZE*(z)+1;
			m_modelDesc[curFace].nIndex2 = (x + 1) + MAPSIZE*(z + 1) + 1;
			m_modelDesc[curFace].nIndex3 = (x + 1) + MAPSIZE*(z)+1;

			m_modelDesc[curFace].tIndex1 = 0 + 1;
			m_modelDesc[curFace].tIndex2 = 2 + 1;
			m_modelDesc[curFace].tIndex3 = 3 + 1;

			curFace += 1;

			m_modelDesc[curFace].vIndex1 = (x)+MAPSIZE*(z)+1;
			m_modelDesc[curFace].vIndex2 = (x) + MAPSIZE*(z + 1) + 1;
			m_modelDesc[curFace].vIndex3 = (x + 1) + MAPSIZE*(z+1)+1;

			m_modelDesc[curFace].nIndex1 = (x)+MAPSIZE*(z)+1;
			m_modelDesc[curFace].nIndex2 = (x) + MAPSIZE*(z + 1) + 1;
			m_modelDesc[curFace].nIndex3 = (x + 1) + MAPSIZE*(z+1)+1;

			m_modelDesc[curFace].tIndex1 = 0 + 1;
			m_modelDesc[curFace].tIndex2 = 1 + 1;
			m_modelDesc[curFace].tIndex3 = 2 + 1;

			curFace += 1;
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

	int flip = 0;

	for (int i = 0; i != uniqueFaceCount; ++i)
	{
		vt1.x = uniqueVertices[m_modelDesc[i].vIndex1-1].x;
		vt1.y = uniqueVertices[m_modelDesc[i].vIndex1-1].y;
		vt1.z = uniqueVertices[m_modelDesc[i].vIndex1 - 1].z;

		vt2.x = uniqueVertices[m_modelDesc[i].vIndex2 - 1].x;
		vt2.y = uniqueVertices[m_modelDesc[i].vIndex2 - 1].y;
		vt2.z = uniqueVertices[m_modelDesc[i].vIndex2 - 1].z;

		vt3.x = uniqueVertices[m_modelDesc[i].vIndex3 - 1].x;
		vt3.y = uniqueVertices[m_modelDesc[i].vIndex3 - 1].y;
		vt3.z = uniqueVertices[m_modelDesc[i].vIndex3 - 1].z;

		D3DXVec3Subtract(&edge1, &vt3, &vt1);
		D3DXVec3Subtract(&edge2, &vt2, &vt1);

		D3DXVec3Cross(&normal, &edge2, &edge1);

		D3DXVec3Normalize(&normal,&normal);

		uniqueNormals[m_modelDesc[i].nIndex1 - 1].x += normal.x;
		uniqueNormals[m_modelDesc[i].nIndex1 - 1].y += normal.y;
		uniqueNormals[m_modelDesc[i].nIndex1 - 1].z += normal.z;

		uniqueNormals[m_modelDesc[i].nIndex2 - 1].x += normal.x;
		uniqueNormals[m_modelDesc[i].nIndex2 - 1].y += normal.y;
		uniqueNormals[m_modelDesc[i].nIndex2 - 1].z += normal.z;

		uniqueNormals[m_modelDesc[i].nIndex3 - 1].x += normal.x;
		uniqueNormals[m_modelDesc[i].nIndex3 - 1].y += normal.y;
		uniqueNormals[m_modelDesc[i].nIndex3 - 1].z += normal.z;

	}

	for (int i = 0; i != uniqueFaceCount; i += 1)
	{
		uniqueNormals[m_modelDesc[i].nIndex1 - 1].x /= 6;
		uniqueNormals[m_modelDesc[i].nIndex1 - 1].y /= 6;
		uniqueNormals[m_modelDesc[i].nIndex1 - 1].z /= 6;

		uniqueNormals[m_modelDesc[i].nIndex2 - 1].x /= 6;
		uniqueNormals[m_modelDesc[i].nIndex2 - 1].y /= 6;
		uniqueNormals[m_modelDesc[i].nIndex2 - 1].z /= 6;

		uniqueNormals[m_modelDesc[i].nIndex3 - 1].x /= 6;
		uniqueNormals[m_modelDesc[i].nIndex3 - 1].y /= 6;
		uniqueNormals[m_modelDesc[i].nIndex3 - 1].z /= 6;
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

	indexX = (x + MAPSIZE*m_gridSize / 2) / m_gridSize;
	indexY = (y + MAPSIZE*m_gridSize / 2) / m_gridSize;

	weightX = (x - (indexX - MAPSIZE / 2) * m_gridSize)/m_gridSize;
	weightY = (y - (indexY - MAPSIZE / 2) * m_gridSize)/m_gridSize;


	avgZ = ((1-weightX)*(1-weightY)*terrainMap[indexX][indexY] + 
			(1-weightX)*(weightY)*terrainMap[indexX][indexY+1] +
			(weightX)*(weightY)*terrainMap[indexX + 1][indexY + 1] +
			(weightX)*(1-weightY)*terrainMap[indexX+1][indexY]);

	return avgZ;
}

float Terrain::GetNormalForLocation(float x, float y)
{
	int indexX, indexY;
	float avgZ;
	float weightX;
	float weightY;

	indexX = (x + MAPSIZE*m_gridSize / 2) / m_gridSize;
	indexY = (y + MAPSIZE*m_gridSize / 2) / m_gridSize;

	weightX = (x - (indexX - MAPSIZE / 2) * m_gridSize) / m_gridSize;
	weightY = (y - (indexY - MAPSIZE / 2) * m_gridSize) / m_gridSize;


	avgZ = ((1 - weightX)*(1 - weightY)*terrainMap[indexX][indexY] +
		(1 - weightX)*(weightY)*terrainMap[indexX][indexY + 1] +
		(weightX)*(weightY)*terrainMap[indexX + 1][indexY + 1] +
		(weightX)*(1 - weightY)*terrainMap[indexX + 1][indexY]);

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
			if (dist < 1) dist = 1;
			if (dist < radius)
			{
				terrainMap[x][z] += METERS(height/sqrt(dist));
			}
		}
	}
	//SmoothTerrainMap(xLoc, zLoc, radius);
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
				terrainMap[x][z] = METERS(offset);
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

