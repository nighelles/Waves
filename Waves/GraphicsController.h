#pragma once

#include <windows.h>
#include "RenderController.h"

#include "Camera.h"
#include "EntityModel.h"
#include "TextureShader.h"
#include "Light.h"

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
	bool Frame();

private:
	bool Render(float);

private:
	RenderController* m_Render;
	Camera* m_PlayerCamera;
	EntityModel* m_TestEntity;
	TextureShader* m_DefaultShader;
	Light* m_Light;

	float m_clearColor;
};