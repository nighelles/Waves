#include "GraphicsController.h"


GraphicsController::GraphicsController()
{
	m_Render = 0;
	m_PlayerCamera = 0;
	m_TestEntity = 0;
	m_DefaultShader = 0;
	m_Light = 0;
	m_clearColor = 0.0f;
}


GraphicsController::GraphicsController(const GraphicsController& other)
{
}


GraphicsController::~GraphicsController()
{
}


bool GraphicsController::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	m_Render = new RenderController;
	if (!m_Render) return false;
	

	result = m_Render->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	m_PlayerCamera = new Camera;
	if (!m_PlayerCamera) return false;
	m_PlayerCamera->SetPosition(0.0f, 0.0f, -30.0f);

	m_TestEntity = new EntityModel;
	if (!m_TestEntity) return false;

	result = m_TestEntity->Initialize(m_Render->GetDevice(), "Boat.obj", L"wood_tiling.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not Initialize Model", L"Error", MB_OK);
		return false;
	}

	m_DefaultShader = new TextureShader;
	if (!m_DefaultShader) return false;
	m_DefaultShader->Initialize(m_Render->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not Initialize Shader", L"Error", MB_OK);
		return false;
	}

	m_Light = new Light;
	if (!m_Light) return false;
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.5f, -0.5f, 1.0f);

	return true;
}


void GraphicsController::Shutdown()
{
	if (m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}
	if (m_DefaultShader)
	{
		m_DefaultShader->Shutdown();
		delete m_DefaultShader;
		m_DefaultShader = 0;
	}
	if (m_TestEntity)
	{
		m_TestEntity->Shutdown();
		delete m_TestEntity;
		m_TestEntity = 0;
	}
	if (m_PlayerCamera)
	{
		delete m_PlayerCamera;
		m_PlayerCamera = 0;
	}
	if (m_Render)
	{
		m_Render->Shutdown();
		delete m_Render;
		m_Render = 0;
	}
	return;
}


bool GraphicsController::Frame()
{
	bool result;
	static float rotation = 0.0f;

	rotation += (float)D3DX_PI*0.01f;
	if (rotation > 360.0f) rotation -= 360.0f;

	result = Render(rotation);
	if (!result) return false;

	return true;
}


bool GraphicsController::Render(float rotation)
{
	D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix;
	bool result;

	m_Render->ClearBuffers(m_clearColor, m_clearColor, m_clearColor, 1.0f);
	
	m_PlayerCamera->Render();
	m_PlayerCamera->GetViewMatrix(viewMatrix);
	m_Render->GetWorldMatrix(worldMatrix);
	m_Render->GetProjectionMatrix(projectionMatrix);

	D3DXMatrixRotationY(&worldMatrix, rotation);
	
	m_TestEntity->Render(m_Render->GetDeviceContext());
	result = m_DefaultShader->Render(m_Render->GetDeviceContext(), m_TestEntity->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_TestEntity->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor());
	if (!result) return false;

	m_Render->PresentBackBuffer();

	return true;
}