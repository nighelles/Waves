#include "EntityModel.h"
#include "ObjFileUtility.h"

#include <atldef.h>
#include <atlstr.h>


EntityModel::EntityModel()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;

	m_textureIndex = 0;

	m_locationX = 0.0f;
	m_locationY = 0.0f;
	m_locationZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;

	m_shaderType = TEXTURE_SHADER;

	m_numFrames = 1;
	m_currentFrame = 0;

	m_fps = 24;

	m_animating = false;

	m_currentTime = 0;
	m_maxTime = 1;

	m_isVisible = true;

	m_materials = 0;
	m_subModels = 0;
}

EntityModel::EntityModel(const EntityModel& other)
{
}

EntityModel::~EntityModel()
{
}

bool EntityModel::Initialize(ID3D11Device* device, char* textureFilename)
{
	bool result;

	m_device = device;

	result = loadBTWFile(textureFilename);
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

void EntityModel::Render(ID3D11DeviceContext* deviceContext, float dt, int submodelNum)
{

	m_maxTime = (float)m_numFrames / (float)m_fps;

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
		UpdateBuffers(m_device, submodelNum);
	}

	RenderBuffers(deviceContext);

	return;
}

int EntityModel::GetIndexCount(int i)
{
	return (m_subModels[i].end-m_subModels[i].end)*3;
}

ID3D11ShaderResourceView* EntityModel::GetTexture(int subModelNum)
{
	return m_materials[subModelNum].texture->GetTexture();
}

bool EntityModel::UpdateBuffers(ID3D11Device* device, int submodelNum)
{
	Vertex* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int i, vIndex, tIndex, nIndex;

	SubModel subModel = m_subModels[submodelNum];

	if (subModel.end == 0) subModel.end = uniqueFaceCount;

	int subModelFaces = subModel.end - subModel.begin;

	vertices = new Vertex[subModelFaces * 3];
	if (!vertices) return false;

	indices = new unsigned long[subModelFaces * 3];
	if (!indices) return false;

	for (i = 0; i != subModelFaces * 3; ++i)
	{
		indices[i] = i;
	}

	UniqueVertex *usedVertices = &(uniqueVertices[m_currentFrame*uniqueVertexCount]);
	UniqueVertex *usedNormals = &(uniqueNormals[m_currentFrame*uniqueNormalCount]);
	UniqueVertex *usedTexcoords = &(uniqueTexcoords[m_currentFrame*uniqueTextureCount]);

	for (int i = subModel.begin; i != subModel.end; ++i)
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
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * subModelFaces * 3;
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
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * subModelFaces * 3;
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

	return true;
}

void EntityModel::ReleaseTexture()
{
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
	if (m_subModels)
	{
		delete[] m_subModels;
		m_subModels = 0;
	}
	if (m_materials)
	{
		delete[] m_materials;
		m_materials = 0;
	}
	return;
}

bool EntityModel::LoadModel(char* filename)
{
	bool result;
	ObjFileUtility *objLoader = new ObjFileUtility;
	int a;

	objLoader->loadStats(filename, uniqueVertexCount, uniqueTextureCount, uniqueNormalCount, uniqueFaceCount, a);

	m_modelDesc = new UniqueFace[uniqueFaceCount];
	uniqueVertices = new UniqueVertex[uniqueVertexCount];
	uniqueTexcoords = new UniqueVertex[uniqueTextureCount];
	uniqueNormals = new UniqueVertex[uniqueNormalCount];

	result = objLoader->LoadObjFile(filename, "garbage.txt", uniqueVertices, uniqueTexcoords, uniqueNormals, m_modelDesc, nullptr);

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

bool EntityModel::loadBTWFile(char* filename)
{
	ifstream fin;

	fin.open(filename);
	if (fin.fail()) return false;

	char matfilename[256];
	char garbage[256];

	fin >> m_subModelCount;

	m_subModels = new SubModel[m_subModelCount];
	m_materials = new Material[m_subModelCount];

	for (int i = 0; i != m_subModelCount; ++i)
	{
		fin >> garbage >> matfilename >> m_subModels[i].begin >> m_subModels[i].end;
		m_materials[i].texture = new Texture;
		m_materials[i].texture->Initialize(m_device, ATL::CA2W(matfilename));
	}

	fin.close();

	return true;
}
