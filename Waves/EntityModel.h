#pragma once

#include <d3d11.h>
#include <d3dx10math.h>
#include <fstream>

#include "Texture.h"

using namespace std;

class EntityModel
{
private:
	struct Vertex
	{
		D3DXVECTOR3 position;
		D3DXVECTOR2 texture;
		D3DXVECTOR3 normal;
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

	ID3D11ShaderResourceView* GetTexture();

protected:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

	virtual bool LoadModel(char*);
	void ReleaseModel();

protected:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;

	Texture* m_Texture; // ADD multiple textures?

	Model* m_model;
};
