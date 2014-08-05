#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include <iostream>
#include <fstream>

#include "NetworkSyncController.h"
#include "NetworkController.h"
#include "NetworkClient.h"
#include "NetworkServer.h"

#include "InputController.h"
#include "GraphicsController.h"
#include "PhysicsEntity.h"
#include "ProceduralTerrain.h"
#include "Camera.h"

#define USE_NETWORKING 0
#define EDITOR_BUILD 1
#define GAME_BUILD 0

class Engine
{
public:
	enum GameState { GAME_MENU, GAME_LOADING, GAME_PLAYING, GAME_PAUSED, GAME_EDITOR };

	typedef struct  
	{
		bool forward, backward, left, right;
		int mouseDX, mouseDY;
	} NetworkPlayer;

public:
	Engine();
	Engine(const Engine&);
	~Engine();

	bool Initialize();
	bool LoadConfiguration();
	bool InitializeGame();

	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Update();
	bool Render();
	bool PostUpdate();

	void PrepareToExit();

	void UpdateEntities();

	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputController* m_Input;
	GraphicsController* m_Graphics;

	PhysicsEntity* m_playerBoat;
	PhysicsEntity* m_otherBoat;

	ProceduralTerrain* m_waterTerrain;
	Terrain* m_landTerrain;

	Bitmap* m_menuBitmap;
	Bitmap* m_menuCursor;

	int m_Time;
	int m_oldTime;

	NetworkServer* m_server;
	NetworkClient* m_client;

	NetworkSyncController* m_networkSyncController;

	NetworkPlayer m_networkPlayer;

	bool m_isServer;
	char m_serverAddress[16];

	GameState m_gameState;

	char m_terrainMapFilename[256];

#if EDITOR_BUILD

	PhysicsEntity* m_editCursor;

	bool m_editedSomething;

#endif // #if EDITOR_BUILD
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static Engine* ApplicationHandle = 0;