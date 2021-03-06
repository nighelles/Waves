#include "GraphicsController.h"

#include <atldef.h>
#include <atlstr.h>

GraphicsController::GraphicsController()
{
	m_Render = 0;
	m_PlayerCamera = 0;
	
	m_DefaultShader = 0;
	m_TerrainShader = 0;
	m_modelShader = 0;

	m_WaterShader = 0;
	m_SkyboxShader = 0;

	m_Light = 0;
	m_clearColor = 0.0f;
	
	m_numEntities = 0;
	m_skybox = 0;
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

int GraphicsController::InitializeEntityModel(char* modelFilename, char* textureFilename)
{
	bool result;

	m_modelEntities[m_numEntities] = new EntityModel;
	if (!m_modelEntities[m_numEntities]) return -1;

	result = m_modelEntities[m_numEntities]->LoadModel(modelFilename);
	if (!result)
	{
		return -1;
	}
	result = m_modelEntities[m_numEntities]->Initialize(m_Render->GetDevice(), textureFilename);
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

	m_TerrainShader = new TerrainShader;
	if (!m_TerrainShader) return false;
	m_TerrainShader->Initialize(m_Render->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not Initialize Terrain Shader", L"Error", MB_OK);
		return false;
	}

	m_modelShader = new ModelShader;
	if (!m_modelShader) return false;
	m_modelShader->Initialize(m_Render->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not Initialize Model Shader", L"Error", MB_OK);
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
	if (m_TerrainShader)
	{
		m_TerrainShader->Shutdown();
		delete m_TerrainShader;
		m_TerrainShader = 0;
	}
	if (m_modelShader)
	{
		m_modelShader->Shutdown();
		delete m_modelShader;
		m_modelShader = 0;
	}
	if (m_WaterShader)
	{
		m_WaterShader->Shutdown();
		delete m_WaterShader;
		m_WaterShader = 0;
	}
	if (m_SkyboxShader)
	{
		m_SkyboxShader->Shutdown();
		delete m_SkyboxShader;
		m_SkyboxShader = 0;
	}
	if (m_skybox)
	{
		m_skybox->Shutdown();
		delete m_skybox;
		m_skybox = 0;
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


bool GraphicsController::Frame(float time)
{
	bool result;
	static float rotation = 0.0f;
	
	m_timeLoopCompletion = time;

	return true;
}

Camera* GraphicsController::GetPlayerCamera()
{
	return m_PlayerCamera;
}

// Time here is between 0 and 1
bool GraphicsController::Render(float dt)
{
	D3DXMATRIX viewMatrix, projectionMatrix, worldMatrix, orthoMatrix;
	bool result = false;
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
			if (m_modelEntities[i]->m_shaderType == EntityModel::TEXTURE_SHADER)
			{
				if (m_modelEntities[i]->usesModelShader)
				{
					m_modelEntities[i]->Render(m_Render->GetDeviceContext(), dt);
					result = m_modelShader->Render(m_Render->GetDeviceContext(),
						m_modelEntities[i]->m_subModels,
						worldMatrix, viewMatrix, projectionMatrix,
						m_modelEntities[i]->GetMaterial(),
						m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Light->GetFillColor(),
						m_modelEntities[i]->m_subModelCount);
				}
				else {
					// Terrain
					m_modelEntities[i]->Render(m_Render->GetDeviceContext(), dt);
					result = m_TerrainShader->Render(m_Render->GetDeviceContext(),
						m_modelEntities[i]->m_indexCount,
						worldMatrix, viewMatrix, projectionMatrix,
						((Terrain*)m_modelEntities[i])->GetTexture(0), ((Terrain*)m_modelEntities[i])->GetTexture(1),
						m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Light->GetFillColor());
				}
			}
			else if (m_modelEntities[i]->m_shaderType == EntityModel::WATER_SHADER)
			{
				result = m_WaterShader->Render(m_Render->GetDeviceContext(),
					m_modelEntities[i]->GetIndexCount(),
					worldMatrix, viewMatrix, projectionMatrix,
					m_modelEntities[i]->GetTexture(0),
					m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Light->GetFillColor(),
					m_timeLoopCompletion);
			}
			if (!result) return false;
		}
	}

	float x, y, z;
	m_PlayerCamera->GetLocation(x, y, z);
	D3DXMatrixTranslation(&worldMatrix, x, y, z);
	

	m_skybox->Render(m_Render->GetDeviceContext(), dt);
	result = m_SkyboxShader->Render(m_Render->GetDeviceContext(), m_skybox->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, m_skybox->GetTexture(0));

	m_Render->DisableZBuffer();
	m_PlayerCamera->GetHUDViewMatrix(viewMatrix);
	for (i = 0; i != m_numBitmaps; ++i)
	{
		if (m_bitmaps[i]->GetVisible())
		{
			m_Render->GetWorldMatrix(worldMatrix);
			m_Render->GetOrthoMatrix(orthoMatrix);
			//m_Render->GetOrthoMatrix(viewMatrix);

			m_bitmaps[i]->Render(m_Render->GetDeviceContext());
			result = m_DefaultShader->Render(m_Render->GetDeviceContext(), m_bitmaps[i]->GetIndexCount(), worldMatrix, viewMatrix, orthoMatrix, m_bitmaps[i]->GetTexture(), m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Light->GetFillColor());
		}
	}

	m_Render->EnableZBuffer();

	m_Render->PresentBackBuffer();

	return true;
}

bool GraphicsController::InitializeSkybox(char* skyboxFilename)
{
	bool result;
	
	// skybox

	m_SkyboxShader = new SkyboxShader;
	if (!m_SkyboxShader) return false;
	result = m_SkyboxShader->Initialize(m_Render->GetDevice(), m_hwnd);
	if (!result)
	{
		MessageBox(m_hwnd, L"Could not Initialize Skybox Shader", L"Error", MB_OK);
		return false;
	}

	m_skybox = new EntityModel;

	result = m_skybox->LoadModel("skybox.obj");
	if (!result)
	{
		MessageBox(m_hwnd, L"Could not Load Skybox Model", L"Error", MB_OK);
		return false;
	}

	result = m_skybox->Initialize(m_Render->GetDevice(), "resources\\entities\\skybox.btw");
	if (!result)
	{
		MessageBox(m_hwnd, L"Could not Initialize Skybox Model", L"Error", MB_OK);
		return false;
	}
	return true;
}
