#include "GraphicsController.h"


GraphicsController::GraphicsController()
{
	m_Render = 0;
	m_PlayerCamera = 0;
	m_TestEntity = 0;
	m_DefaultShader = 0;

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
	m_PlayerCamera->SetPosition(0.0f, 0.0f, -10.0f);

	m_TestEntity = new Entity;
	if (!m_TestEntity) return false;
	m_TestEntity->Initialize();

	result = m_TestEntity->InitializeModel(m_Render->GetDevice(), L"water_tiling.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not Initialize Model", L"Error", MB_OK);
	}

	m_DefaultShader = new TextureShader;
	if (!m_DefaultShader) return false;
	m_DefaultShader->Initialize(m_Render->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not Initialize Shader", L"Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsController::Shutdown()
{
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

	result = Render();
	if (!result) return false;

	return true;
}


bool GraphicsController::Render()
{
	D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix;
	bool result;

	m_Render->ClearBuffers(m_clearColor, m_clearColor, m_clearColor, 1.0f);
	
	m_PlayerCamera->Render();
	m_PlayerCamera->GetViewMatrix(viewMatrix);
	m_Render->GetWorldMatrix(worldMatrix);
	m_Render->GetProjectionMatrix(projectionMatrix);
	
	m_TestEntity->Render(m_Render->GetDeviceContext());
	result = m_DefaultShader->Render(m_Render->GetDeviceContext(), m_TestEntity->GetModel()->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_TestEntity->GetModel()->GetTexture());
	if (!result) return false;

	m_Render->PresentBackBuffer();

	return true;
}