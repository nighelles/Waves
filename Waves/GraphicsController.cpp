#include "GraphicsController.h"


GraphicsController::GraphicsController()
{
	m_Render = 0;
	m_PlayerCamera = 0;
	
	m_DefaultShader = 0;
	m_WaterShader = 0;

	m_Light = 0;
	m_clearColor = 0.0f;
	
	m_numEntities = 0;
}


GraphicsController::GraphicsController(const GraphicsController& other)
{
}


GraphicsController::~GraphicsController()
{
}

RenderController* GraphicsController::GetRenderController()
{
	return m_Render;
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

// This is the new method, depreciating old method
int GraphicsController::RegisterEntityModel(EntityModel* model)
{
	m_modelEntities[m_numEntities] = model;
	if (!m_modelEntities[m_numEntities])
	{
		MessageBox(m_hwnd, L"Could not Register Model", L"Error", MB_OK);
		return -1;
	}

	m_numEntities += 1;
	return (m_numEntities - 1);
}

EntityModel* GraphicsController::GetEntityModel(int entityID)
{
	return (m_modelEntities[entityID]);
}

int GraphicsController::RegisterBitmap(Bitmap* bitmap)
{
	bool result;

	m_bitmaps[m_numBitmaps] = bitmap;
	if (!m_bitmaps[m_numBitmaps]) 
	{
		MessageBox(m_hwnd, L"Could not Initialize Bitmap", L"Error", MB_OK);
		return -1;
	}

	m_numBitmaps += 1;
	return (m_numBitmaps - 1);
}

bool GraphicsController::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

	m_hwnd = hwnd;
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

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
	m_PlayerCamera->SetLocation(0.0f, 0.0f, -1.0f);

	m_DefaultShader = new TextureShader;
	if (!m_DefaultShader) return false;
	m_DefaultShader->Initialize(m_Render->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not Initialize Default Shader", L"Error", MB_OK);
		return false;
	}

	m_WaterShader = new ProceduralTerrainShader;
	if (!m_WaterShader) return false;
	m_WaterShader->Initialize(m_Render->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not Initialize Water Shader", L"Error", MB_OK);
		return false;
	}

	m_Light = new Light;
	if (!m_Light) return false;
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetFillColor (0.3f, 0.3f, 0.3f, 1.0f);
	m_Light->SetDirection(0.0f, -1.0f, 0.0f);

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
	if (m_WaterShader)
	{
		m_WaterShader->Shutdown();
		delete m_WaterShader;
		m_WaterShader = 0;
	}
	// We don't own these, so just set them to zero
	for (int i = 0; i != m_numEntities; ++i)
	{
		m_modelEntities[i] = 0;
	}
	for (int i = 0; i != m_numBitmaps; ++i)
	{
		m_bitmaps[i] = 0;
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
	
	m_timeLoopCompletion = time;

	m_PlayerCamera->ApplyRotation(mouseDY/2.0f,mouseDX/2.0f,0.0f);

	return true;
}

Camera* GraphicsController::GetPlayerCamera()
{
	return m_PlayerCamera;
}

// Time here is between 0 and 1
bool GraphicsController::Render()
{
	D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix, orthoMatrix;
	bool result;
	int i;

	m_Render->ClearBuffers(m_clearColor, m_clearColor, m_clearColor, 1.0f);
	
	m_PlayerCamera->Render();
	m_PlayerCamera->GetViewMatrix(viewMatrix);
	m_Render->GetWorldMatrix(worldMatrix);
	m_Render->GetProjectionMatrix(projectionMatrix);
	
	for (i = 0; i != m_numEntities; ++i)
	{
		if (m_modelEntities[i]->IsVisible())
		{
			m_Render->GetWorldMatrix(worldMatrix);

			m_modelEntities[i]->ApplyEntityMatrix(worldMatrix);
			m_modelEntities[i]->Render(m_Render->GetDeviceContext());
			if (m_modelEntities[i]->m_shaderType == EntityModel::TEXTURE_SHADER)
			{
				result = m_DefaultShader->Render(m_Render->GetDeviceContext(), m_modelEntities[i]->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_modelEntities[i]->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Light->GetFillColor());
			}
			else if (m_modelEntities[i]->m_shaderType == EntityModel::WATER_SHADER)
			{
				result = m_WaterShader->Render(m_Render->GetDeviceContext(), m_modelEntities[i]->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_modelEntities[i]->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Light->GetFillColor(), m_timeLoopCompletion);
			} 
			if (!result) return false;
		}
	}

	m_Render->DisableZBuffer();

	for (i = 0; i != m_numBitmaps; ++i)
	{
		if (m_bitmaps[i]->GetVisible())
		{
			m_Render->GetWorldMatrix(worldMatrix);
			m_Render->GetOrthoMatrix(orthoMatrix);

			m_bitmaps[i]->Render(m_Render->GetDeviceContext(), 0, 0);
			result = m_DefaultShader->Render(m_Render->GetDeviceContext(), m_bitmaps[i]->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_bitmaps[i]->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Light->GetFillColor());
		}
	}

	m_Render->EnableZBuffer();

	m_Render->PresentBackBuffer();

	return true;
}