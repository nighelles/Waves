#pragma once

#include <d3d11.h>
#include <d3dx10math.h>
#include <fstream>

#include "Texture.h"

#include <stdio.h>
#include <iostream>

#define FRAMES_PER_SECOND 24.0f

#define MAX_TEXTURES 5

using namespace std;

class EntityModel
{
public:
	enum ShaderType { TEXTURE_SHADER=1, WATER_SHADER };

public:
	struct Vertex
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
	};

	struct UniqueVertex
	{
		float x, y, z;
	};

	struct UniqueFace
	{
		int vIndex1, vIndex2, vIndex3;
		int nIndex1, nIndex2, nIndex3;
		int tIndex1, tIndex2, tIndex3;
	};

	struct Material
	{
		Texture *texture;
	};

	struct SubModel
	{
		int begin;
		int end;
	};

public:
	struct Model
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

public:
	EntityModel();
	EntityModel(const EntityModel&);
	~EntityModel();

	virtual bool Initialize(ID3D11Device*, char*);
	virtual void Shutdown();
	void Render(ID3D11DeviceContext*, float dt);

	int GetIndexCount();
	void SetLocation(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	void ApplyEntityMatrix(D3DXMATRIX& entityMatrix);

	ID3D11ShaderResourceView* GetTexture();
	Material* GetMaterial();

	ShaderType m_shaderType;

	bool IsVisible() const { return m_isVisible; }
	void IsVisible(bool val) { m_isVisible = val; }

	// Function to load a model file

	bool loadBinaryFile(char* filename);
	bool writeBinaryFile(char* filename);

	bool loadBTWFile(char* filename);

	virtual bool LoadModel(char*);

	// Animation functions

	bool Animating() const { return m_animating; }
	void Animating(bool val) { m_animating = val; }
	int CurrentFrame() const { return m_currentFrame; }
	void CurrentFrame(int val) { m_currentFrame = val; }
	int Fps() const { return m_fps; }
	void Fps(int val) { m_fps = val; }

	int m_vertexCount, m_indexCount;
	int uniqueVertexCount, uniqueTextureCount, uniqueNormalCount, uniqueFaceCount;
	int m_subModelCount;
	
	UniqueVertex *uniqueVertices, *uniqueTexcoords, *uniqueNormals;
	UniqueFace* m_modelDesc;

	SubModel *m_subModels;
	Material *m_materials;

	bool usesModelShader;

protected:
	virtual bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	virtual void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

	void ReleaseModel();

protected:
	int m_currentFrame;
	int m_numFrames;

	bool m_animating;

	int m_fps;
	float m_currentTime;
	float m_maxTime;

	bool m_isVisible;
	bool m_inWorld;

	float m_locationX, m_locationY, m_locationZ;
	float m_rotationX, m_rotationY, m_rotationZ;

	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;

	int		 m_textureIndex;

	ID3D11Device* m_device;
};
