#pragma once

#include <d3d11.h>
#include <d3dx10math.h>
#include <fstream>

#include "Texture.h"

#include "raytriangle.h"


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
		int tIndex1, tIndex2, tIndex3;
		int nIndex1, nIndex2, nIndex3;
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

	virtual bool Initialize(ID3D11Device*, char*, WCHAR*);
	virtual void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	void SetLocation(float x, float y, float z);
	void SetRotation(float x, float y, float z);

	void ApplyEntityMatrix(D3DXMATRIX& entityMatrix);

	ID3D11ShaderResourceView* GetTexture();

	ShaderType m_shaderType;

	bool GetRayIntersection(D3DXVECTOR3* orig, D3DXVECTOR3* dir, D3DXVECTOR3& point);

	bool IsVisible() const { return m_isVisible; }
	void IsVisible(bool val) { m_isVisible = val; }

protected:
	virtual bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	virtual void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

	virtual bool LoadModel(char*);
	void ReleaseModel();
	bool BuildModel();

protected:
	bool m_isVisible;

	float m_locationX, m_locationY, m_locationZ;
	float m_rotationX, m_rotationY, m_rotationZ;

	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;

	Texture* m_Texture; // ADD multiple textures?

	ID3D11Device* m_device;

	Model* m_model;

	UniqueVertex *uniqueVertices, *uniqueTexcoords, *uniqueNormals;
	UniqueFace* m_modelDesc;
	int uniqueVertexCount, uniqueTextureCount, uniqueNormalCount, uniqueFaceCount;
};
