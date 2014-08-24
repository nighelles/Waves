#pragma once

#include <d3d11.h>
#include <d3dx10math.h>
#include <d3dx11async.h>
#include <fstream>

#include "EntityModel.h"

using namespace std;

class ModelShader
{
protected:
	struct MatrixBuffer
	{
		D3DXMATRIX world;
		D3DXMATRIX view;
		D3DXMATRIX projection;
		D3DXMATRIX local;
	};
	struct LightBuffer
	{
		D3DXVECTOR4 diffuseColor;
		D3DXVECTOR4 fillColor;
		D3DXVECTOR3 lightDirection;
		float padding;
	};
public:
	ModelShader();
	ModelShader(const ModelShader&);
	~ModelShader();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*,
		EntityModel::SubModel* subModels,
		D3DXMATRIX,
		D3DXMATRIX,
		D3DXMATRIX,
		EntityModel::Material*,
		D3DXVECTOR3,
		D3DXVECTOR4,
		D3DXVECTOR4,
		int numSubmodels);

protected:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);

	bool SetShaderParameters(
		ID3D11DeviceContext*, 
		D3DXMATRIX,
		D3DXMATRIX, 
		D3DXMATRIX, 
		D3DXVECTOR3,
		D3DXVECTOR4, 
		D3DXVECTOR4);

	void RenderShader(ID3D11DeviceContext*);

protected:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;

	ID3D11SamplerState* m_sampleState;
	ID3D11BlendState* m_blendState;

	ID3D11Buffer* m_lightBuffer;
};
