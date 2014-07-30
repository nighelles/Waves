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
	D3D11_SUBRESOURCE_DATA vertexData;
}

void ProceduralTerrain::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	Vertex* vertices;
	unsigned long* indices;
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(Vertex);
	offset = 0;

	//UPDATE VERTEX BUFFER
	/*
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

	//END VERTEX BUFFER
	*/

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool ProceduralTerrain::InitializeBuffers(ID3D11Device* device)
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
	m_vertexCount = 6000000;
	m_indexCount = m_vertexCount;

	m_model = new Model[m_indexCount];

	for (int x = 0; x != 1000; ++x) // generate initial map heights
	{
		for (int z = 0; z != 1000; ++z)
		{
			terrainMap[x][z] = 0.0;
		}
	}

	regenerateModelFromTerrainMap();

	return true;
}

float ProceduralTerrain::CalculateDeterministicHeight(float x, float y, float t)
{
	// Rolling waves with a slower wave
	return 5.0*sin(6.28*x / 100.0 + 3.14 * 5 * t) +10.0*sin(3.14*x / 100.0 + 2 * 3.14*t);
	//Rolling waves intersecting in x and y
	//return 5.0*sin(6.28*x / 100.0 + 3.14 * 5 * t) + 5.0*sin(6.28*y / 100.0 + 3.14 * 5 * t);
	//return 5.0f*sin(y/20.0f);
}

void ProceduralTerrain::Update(float loopCompletion)
{
	
}

void ProceduralTerrain::regenerateModelFromTerrainMap() 
{
	int index = 0;

	for (int x = 0; x != 999; ++x)
	{
		for (int z = 0; z != 999; ++z)
		{
			m_model[index].x = x*4-2000;
			m_model[index].y = terrainMap[x][z];
			m_model[index].z = z*4 - 2000;
			m_model[index].tu = 0.0f;
			m_model[index].tv = 0.0f;
			m_model[index].nx = 0.0f;
			m_model[index].ny = 1.0f;
			m_model[index].nz = 0.0f;

			m_model[index + 1].x = (x + 1) * 4 - 2000;
			m_model[index+1].y = terrainMap[x+1][z+1];
			m_model[index + 1].z = (z + 1) * 4 - 2000;
			m_model[index+1].tu = 1.0f;
			m_model[index+1].tv = 1.0f;
			m_model[index+1].nx = 0.0f;
			m_model[index+1].ny = 1.0f;
			m_model[index+1].nz = 0.0f;

			m_model[index + 2].x = (x + 1)*4 - 2000;
			m_model[index+2].y = terrainMap[x+1][z];
			m_model[index + 2].z = z* 4 - 2000;
			m_model[index+2].tu = 1.0f;
			m_model[index+2].tv = 0.0f;
			m_model[index+2].nx = 0.0f;
			m_model[index+2].ny = 1.0f;
			m_model[index+2].nz = 0.0f;

			m_model[index + 3].x = x*4 - 2000;;
			m_model[index+3].y = terrainMap[x][z];
			m_model[index + 3].z = z*4 - 2000;
			m_model[index+3].tu = 0.0f;
			m_model[index+3].tv = 0.0f;
			m_model[index+3].nx = 0.0f;
			m_model[index+3].ny = 1.0f;
			m_model[index+3].nz = 0.0f;

			m_model[index + 4].x = x * 4 - 2000;
			m_model[index+4].y = terrainMap[x][z+1];
			m_model[index + 4].z = (z + 1) * 4 - 2000;
			m_model[index+4].tu = 0.0f;
			m_model[index+4].tv = 1.0f;
			m_model[index+4].nx = 0.0f;
			m_model[index+4].ny = 1.0f;
			m_model[index+4].nz = 0.0f;

			m_model[index + 5].x = (x + 1) * 4 - 2000;
			m_model[index+5].y = terrainMap[x+1][z+1];
			m_model[index + 5].z = (z + 1) * 4 - 2000;
			m_model[index+5].tu = 1.0f;
			m_model[index+5].tv = 1.0f;
			m_model[index+5].nx = 0.0f;
			m_model[index+5].ny = 1.0f;
			m_model[index+5].nz = 0.0f;

			index += 6;
		}
	}
}