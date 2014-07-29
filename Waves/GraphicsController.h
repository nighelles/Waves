#pragma once

#include <windows.h>
#include "RenderController.h"

#include "Camera.h"
#include "EntityModel.h"
#include "TextureShader.h"
#include "Light.h"

#include "ProceduralTerrain.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

class GraphicsController
{
public:
	GraphicsController();
	GraphicsController(const GraphicsController&);
	~GraphicsController();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(int, int, int, int, float);
	bool Render();

	int InitializeEntityModel(char* modelFilename, WCHAR* textureFilename);
	EntityModel* GetEntityModel(int entityID);

	Camera* GetPlayerCamera();
	ProceduralTerrain* GetTerrain();

private:
	RenderController* m_Render;
	Camera* m_PlayerCamera;
	TextureShader* m_DefaultShader;
	Light* m_Light;

	EntityModel* m_playerEntity;
	ProceduralTerrain* m_waterTerrain;

	EntityModel* m_modelEntities[10];
	int m_numEntities;

	float m_clearColor;

	HWND m_hwnd;
};