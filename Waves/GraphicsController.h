#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include "RenderController.h"

#include "Camera.h"
#include "EntityModel.h"
#include "Light.h"

#include "Bitmap.h"

#include "ProceduralTerrain.h"

#include "ModelShader.h"
#include "TextureShader.h"
#include "ProceduralTerrainShader.h"
#include "SkyboxShader.h"
#include "TerrainShader.h"

#include "PhysicsDefs.h"

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = METERS(1000);
const float SCREEN_NEAR = 0.1f;

class GraphicsController
{
public:
	GraphicsController();
	GraphicsController(const GraphicsController&);
	~GraphicsController();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(float);
	bool Render(float dt);

	RenderController* GetRenderController();

	int InitializeEntityModel(char* modelFilename, char* textureFilename);
	EntityModel* GetEntityModel(int entityID);

	int RegisterEntityModel(EntityModel* model);

	int RegisterBitmap(Bitmap* bitmap);

	bool InitializeSkybox(char* skyboxFilename);

	Camera* GetPlayerCamera();
	ProceduralTerrain* GetTerrain();

	Light* GetLight() const { return m_Light; }


	// Accessors

private:
	RenderController* m_Render;
	Camera* m_PlayerCamera;

	TextureShader* m_DefaultShader;
	TerrainShader* m_TerrainShader;
	ModelShader* m_modelShader;

	ProceduralTerrainShader* m_WaterShader;
	SkyboxShader* m_SkyboxShader;

	Light* m_Light;

	EntityModel* m_playerEntity;

	EntityModel* m_modelEntities[10];

	EntityModel* m_skybox;
	int m_numEntities;

	Bitmap* m_bitmaps[10];
	int m_numBitmaps;

	float m_clearColor;

	float m_timeLoopCompletion;

	HWND m_hwnd;
	int m_screenWidth, m_screenHeight;
};