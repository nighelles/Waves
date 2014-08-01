#pragma once

#include <windows.h>

#include "InputController.h"
#include "GraphicsController.h"
#include "PhysicsEntity.h"
#include "ProceduralTerrain.h"
#include "Camera.h"

class Engine
{
public:
	enum GameState { GAME_MENU, GAME_LOADING, GAME_PLAYING, GAME_PAUSED };
public:
	Engine();
	Engine(const Engine&);
	~Engine();

	bool Initialize();
	bool InitializeGame();

	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Update();
	bool Render();
	bool PostUpdate();

	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputController* m_Input;
	GraphicsController* m_Graphics;

	PhysicsEntity* m_playerBoat;

	ProceduralTerrain* m_waterTerrain;

	Bitmap* m_menuBitmap;
	Bitmap* m_menuCursor;

	int m_Time;
	int m_oldTime;

	bool m_isServer;

	GameState m_gameState;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static Engine* ApplicationHandle = 0;