#include "GraphicsController.h"


GraphicsController::GraphicsController()
{
	m_Render = 0;
	m_PlayerCamera = 0;
	m_DefaultShader = 0;
	m_Light = 0;
	m_clearColor = 0.0f;

	m_waterTerrain = 0;
	
	m_numEntities = 0;
}


GraphicsController::GraphicsController(const GraphicsController& other)
{
}


GraphicsController::~GraphicsController()
{
}

int GraphicsController::InitializeEntityModel(char* modelFilename, WCHAR* textureFilename)
{
	bool result;

	m_modelEntities[m_numEntities] = new EntityModel;
	if (!m_modelEntities[m_numEntities]) return -1;

	result = m_modelEntities[m_numEntities]->Initialize(m_Render->GetDevice(), modelFilename, textureFilename);
	if (!result)
	{
		MessageBox(m_hwnd, L"Could not Initialize Model", L"Error", MB_OK);
		return -1;
	}

	m_numEntities += 1;
	return (m_numEntities - 1);
}

EntityModel* GraphicsController::GetEntityModel(int entityID)
{
	return (m_modelEntities[entityID]);
}

bool GraphicsController::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	m_hwnd = hwnd;

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
	m_PlayerCamera->SetPosition(0.0f, 2.0f, -30.0f);

	m_waterTerrain = new ProceduralTerrain();
	if (!m_waterTerrain) return false;

	result = m_waterTerrain->Initialize(m_Render->GetDevice(), L"water_tiling.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not Initialize Water Terrain", L"Error", MB_OK);
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
	m_Light->SetFillColor (0.1f, 0.1f, 0.1f, 1.0f);
	m_Light->SetDirection(0.0f,-1.f,0.0f);

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
	for (int i = 0; i != m_numEntities; ++i)
	{
		if (m_modelEntities[i])
		{
			m_modelEntities[i]->Shutdown();
			delete m_modelEntities[i];
			m_modelEntities[i] = 0;
		}
	}
	if (m_waterTerrain)
	{
		m_waterTerrain->Shutdown();
		delete m_waterTerrain;
		m_waterTerrain = 0;
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


bool GraphicsController::Frame(int mouseX, int mouseY, int mouseDX, int mouseDY, float time)
{
	bool result;
	static float rotation = 0.0f;
	
	float loopCompletion = time / 60000.0f;
	m_waterTerrain->Update(loopCompletion);

	m_PlayerCamera->ApplyRotation(mouseDY/2.0f,mouseDX/2.0f,0.0f);

	return true;
}

Camera* GraphicsController::GetPlayerCamera()
{
	return m_PlayerCamera;
}

ProceduralTerrain* GraphicsController::GetTerrain()
{
	return m_waterTerrain;
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
	
	for (int i = 0; i != m_numEntities; ++i)
	{
		m_Render->GetWorldMatrix(worldMatrix);

		m_modelEntities[i]->ApplyEntityMatrix(worldMatrix);
		m_modelEntities[i]->Render(m_Render->GetDeviceContext());
		result = m_DefaultShader->Render(m_Render->GetDeviceContext(), m_modelEntities[i]->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_modelEntities[i]->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Light->GetFillColor());
		if (!result) return false;
	}

	m_Render->GetWorldMatrix(worldMatrix);
	m_waterTerrain->Render(m_Render->GetDeviceContext());
	result = m_DefaultShader->Render(m_Render->GetDeviceContext(), m_waterTerrain->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_waterTerrain->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Light->GetFillColor());
	if (!result) return false;

	m_Render->PresentBackBuffer();

	return true;
}