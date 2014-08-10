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

#include "PlayerEntity.h"

#include "Physics.h"

#define USE_NETWORKING 0
#define EDITOR_BUILD 0
#define GAME_BUILD 1

class Engine
{
public:
	enum GameState { GAME_MENU, GAME_LOADING,GAME_WAIT_NETWORK, GAME_PLAYING, GAME_PAUSED, GAME_EDITOR };

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
	bool InitializeNetworking();
	bool ConnectNetworking();

	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Update();
	bool Render();
	bool PostUpdate();

	void PrepareToExit();

	void UpdateEntities(float dt, float loopCompletion);

	void InitializeWindows(int&, int&);
	void ShutdownWindows();
	void MovePlayer(NetworkedInput inp, PlayerEntity* player, float dt);

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputController* m_Input;
	GraphicsController* m_Graphics;

	PhysicsEntity* m_playerBoat;
	PhysicsEntity* m_otherBoat;

	PlayerEntity* m_player;
	PlayerEntity* m_otherPlayer;

	ProceduralTerrain* m_waterTerrain;
	Terrain* m_landTerrain;

	Bitmap* m_menuBitmap;
	Bitmap* m_networkLoadingBitmap;
	Bitmap* m_menuCursor;

	Bitmap* m_crosshair;

	float m_timeloopCompletion;

	NetworkServer* m_server;
	NetworkClient* m_client;

	bool m_playerConnected;

	NetworkSyncController* m_networkSyncController;

	NetworkPlayer m_networkPlayer;

	bool m_isServer;
	char m_serverAddress[16];

	GameState m_gameState;

	char m_terrainMapFilename[256];

	SYSTEMTIME newtime, oldtime;

#if EDITOR_BUILD

	PhysicsEntity* m_editCursor;

	bool m_editedSomething;

#endif // #if EDITOR_BUILD

};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static Engine* ApplicationHandle = 0;