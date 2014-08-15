#include "EntityModel.h"
#include "ObjFileUtility.h"


EntityModel::EntityModel()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_Texture = 0;

	m_locationX = 0.0f;
	m_locationY = 0.0f;
	m_locationZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;

	m_shaderType = TEXTURE_SHADER;

	m_numFrames = 1;
	m_currentFrame = 0;

	m_animating = false;

	m_currentTime = 0;
	m_maxTime = 1;

	m_isVisible = true;
}

EntityModel::EntityModel(const EntityModel& other)
{
}

EntityModel::~EntityModel()
{
}

bool EntityModel::Initialize(ID3D11Device* device, WCHAR* textureFilename)
{
	bool result;

	m_device = device;

	result = InitializeBuffers(device);
	if (!result) return false;

	result = LoadTexture(device, textureFilename);
	if (!result) return false;
	
	return true;
}

void EntityModel::Shutdown()
{
	ReleaseTexture();

	ShutdownBuffers();

	ReleaseModel();

	return;
}

void EntityModel::Render(ID3D11DeviceContext* deviceContext, float dt)
{
	if (m_animating && m_numFrames > 1)
	{
		m_currentTime += dt;

		if (m_currentTime > m_maxTime)
		{
			m_currentTime = 0;
			m_animating = false;
			m_currentFrame = 0;
		}
		else {
			m_currentFrame = (m_currentTime / m_maxTime)*m_numFrames;
			m_currentTime += dt;
			if (m_currentFrame > m_numFrames) m_currentFrame = m_numFrames;
		}
		InitializeBuffers(m_device);
	}

	RenderBuffers(deviceContext);

	return;
}

int EntityModel::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* EntityModel::GetTexture()
{
	return m_Texture->GetTexture();
}

bool EntityModel::InitializeBuffers(ID3D11Device* device)
{
	Vertex* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i, vIndex, tIndex, nIndex;

	vertices = new Vertex[m_vertexCount];
	if (!vertices) return false;

	indices = new unsigned long[m_indexCount];
	if (!indices) return false;

	for (i = 0; i != uniqueFaceCount*3; ++i)
	{
		indices[i] = i;
	}

	UniqueVertex *usedVertices = &(uniqueVertices[m_currentFrame*uniqueVertexCount]);
	UniqueVertex *usedNormals = &(uniqueNormals[m_currentFrame*uniqueNormalCount]);
	UniqueVertex *usedTexcoords = &(uniqueTexcoords[m_currentFrame*uniqueTextureCount]);

	for (int i = 0; i != uniqueFaceCount; ++i)
	{
		vIndex = m_modelDesc[i].vIndex1 - 1;
		tIndex = m_modelDesc[i].tIndex1 - 1;
		nIndex = m_modelDesc[i].nIndex1 - 1;

		vertices[3 * i].position = D3DXVECTOR3(usedVertices[vIndex].x, usedVertices[vIndex].y, usedVertices[vIndex].z);
		vertices[3 * i].texture = D3DXVECTOR2(usedTexcoords[tIndex].x, usedTexcoords[tIndex].y);
		vertices[3 * i].normal = D3DXVECTOR3(usedNormals[nIndex].x, usedNormals[nIndex].y, usedNormals[nIndex].z);

		vIndex = m_modelDesc[i].vIndex2 - 1;
		tIndex = m_modelDesc[i].tIndex2 - 1;
		nIndex = m_modelDesc[i].nIndex2 - 1;

		vertices[3 * i + 1].position = D3DXVECTOR3(usedVertices[vIndex].x, usedVertices[vIndex].y, usedVertices[vIndex].z);
		vertices[3 * i + 1].texture = D3DXVECTOR2(usedTexcoords[tIndex].x, usedTexcoords[tIndex].y);
		vertices[3 * i + 1].normal = D3DXVECTOR3(usedNormals[nIndex].x, usedNormals[nIndex].y, usedNormals[nIndex].z);

		vIndex = m_modelDesc[i].vIndex3 - 1;
		tIndex = m_modelDesc[i].tIndex3 - 1;
		nIndex = m_modelDesc[i].nIndex3 - 1;

		vertices[3 * i + 2].position = D3DXVECTOR3(usedVertices[vIndex].x, usedVertices[vIndex].y, usedVertices[vIndex].z);
		vertices[3 * i + 2].texture = D3DXVECTOR2(usedTexcoords[tIndex].x, usedTexcoords[tIndex].y);
		vertices[3 * i + 2].normal = D3DXVECTOR3(usedNormals[nIndex].x, usedNormals[nIndex].y, usedNormals[nIndex].z);

	}



	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
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

void EntityModel::ShutdownBuffers()
{
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void EntityModel::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(Vertex);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool EntityModel::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;

	m_Texture = new Texture;
	if (!m_Texture) return false;

	result = m_Texture->Initialize(device, filename);
	if (!result) return false;

	return true;
}

void EntityModel::ReleaseTexture()
{
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}

void EntityModel::ReleaseModel()
{
	if (m_modelDesc)
	{
		delete[] m_modelDesc;
		m_modelDesc = 0;
	}
	if (uniqueVertices)
	{
		delete[] uniqueVertices;
		uniqueVertices = 0;
	}
	if (uniqueNormals)
	{
		delete[] uniqueNormals;
		uniqueNormals = 0;
	}
	if (uniqueTexcoords)
	{
		delete[] uniqueTexcoords;
		uniqueTexcoords = 0;
	}

	return;
}

bool EntityModel::LoadModel(char* filename)
{
	bool result;
	ObjFileUtility *objLoader = new ObjFileUtility;

	objLoader->loadStats(filename, uniqueVertexCount, uniqueTextureCount, uniqueNormalCount, uniqueFaceCount);

	m_modelDesc = new UniqueFace[uniqueFaceCount];
	uniqueVertices = new UniqueVertex[uniqueVertexCount];
	uniqueTexcoords = new UniqueVertex[uniqueTextureCount];
	uniqueNormals = new UniqueVertex[uniqueNormalCount];

	result = objLoader->LoadObjFile(filename, uniqueVertices, uniqueTexcoords, uniqueNormals, m_modelDesc);

	m_vertexCount = uniqueFaceCount * 3;

	m_indexCount = m_vertexCount;

	if (m_vertexCount < 1) return false;

	objLoader->Shutdown();
	delete objLoader;

	return true;
}


void EntityModel::SetLocation(float x, float y, float z)
{
	m_locationX = x;
	m_locationY = y;
	m_locationZ = z;
}

void EntityModel::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
}


void EntityModel::ApplyEntityMatrix(D3DXMATRIX& entityMatrix)
{
	D3DXMATRIX translation, rotation;
	D3DXMatrixTranslation(&translation, m_locationX, m_locationY, m_locationZ);
	D3DXMatrixRotationYawPitchRoll(&rotation, m_rotationY*0.0174532925f, m_rotationX*0.0174532925f, m_rotationZ*0.0174532925f);

	entityMatrix = translation;
	D3DXMatrixMultiply(&entityMatrix, &rotation, &entityMatrix);
	return;
}

long getFileSize(FILE *file)
{
		long lCurPos, lEndPos;
		lCurPos = ftell(file);
		fseek(file, 0, 2);
		lEndPos = ftell(file);
		fseek(file, lCurPos, 0);
		return lEndPos;
}

typedef unsigned char BYTE;

bool EntityModel::loadBinaryFile(char* filename)
{
	bool result;

	BYTE *fileBuf;
	FILE *file = NULL;

	if ((file = fopen(filename, "rb")) == NULL) return false;

	long fileSize = getFileSize(file);

	fileBuf = new BYTE[fileSize];
	fread(fileBuf, fileSize, 1, file);

	// LOAD MODEL INFORMATION
	m_numFrames = ((int*)fileBuf)[0];
	uniqueFaceCount = ((int*)fileBuf)[1];
	uniqueVertexCount = ((int*)fileBuf)[2];
	uniqueNormalCount = ((int*)fileBuf)[3];
	uniqueTextureCount = ((int*)fileBuf)[4];
	
	// LOAD UNIQUE FACES
	int faceDataOffset = sizeof(int) * 5;
	m_modelDesc = new UniqueFace[uniqueFaceCount];

	memcpy((BYTE*)m_modelDesc, (fileBuf + faceDataOffset), sizeof(int) * 9 * uniqueFaceCount);

	// Load frames
	int frameOffset = faceDataOffset + sizeof(int) * 9 * uniqueFaceCount;
	int currentOffset = frameOffset;

	uniqueVertices = new UniqueVertex[m_numFrames*uniqueVertexCount];
	uniqueNormals = new UniqueVertex[m_numFrames*uniqueNormalCount];
	uniqueTexcoords = new UniqueVertex[m_numFrames*uniqueTextureCount];

	memcpy((BYTE*)uniqueVertices, (fileBuf + currentOffset), sizeof(float) * 3 * m_numFrames*uniqueVertexCount);
	currentOffset += sizeof(float) * 3 * m_numFrames*uniqueVertexCount;
	memcpy((BYTE*)uniqueNormals, (fileBuf + currentOffset), sizeof(float) * 3 * m_numFrames*uniqueNormalCount);
	currentOffset += sizeof(float) * 3 * m_numFrames*uniqueNormalCount;
	memcpy((BYTE*)uniqueTexcoords, (fileBuf + currentOffset), sizeof(float) * 3 * m_numFrames*uniqueTextureCount);
	
	delete[] fileBuf;
	fclose(file);

	m_vertexCount = uniqueFaceCount * 3;
	m_indexCount = m_vertexCount;

	m_maxTime = (m_numFrames / FRAMES_PER_SECOND); // 24 FPS

	return true;
}

bool EntityModel::writeBinaryFile(char* filename)
{
	FILE *file = NULL;
	if ((file = fopen(filename, "wb")) == NULL) return false;

	fwrite(&m_numFrames, sizeof(int), 1, file);
	fwrite(&uniqueFaceCount, sizeof(int), 1, file);
	fwrite(&uniqueVertexCount, sizeof(int), 1, file);
	fwrite(&uniqueNormalCount, sizeof(int), 1, file);
	fwrite(&uniqueTextureCount, sizeof(int), 1, file);
	fwrite(m_modelDesc, sizeof(int), uniqueFaceCount * 9, file);
	fwrite(uniqueVertices, sizeof(float), uniqueVertexCount * 3 * m_numFrames, file);
	fwrite(uniqueNormals, sizeof(float), uniqueNormalCount * 3 * m_numFrames, file);
	fwrite(uniqueTexcoords, sizeof(float), uniqueTextureCount * 3 * m_numFrames, file);

	fclose(file);

	return true;
}
