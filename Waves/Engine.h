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

#define USE_NETWORKING 1
#define EDITOR_BUILD 0
#define GAME_BUILD 1

#define NUMBEROFTEAMS 2

#define MAXSPAWNPOINTS 10
#define MAXENTITIES 10

#define MAXPLAYERS 5

#define MAXWEAPONS 3

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment( lib, "ws2_32.lib")

class Engine
{
public:
	enum GameState { GAME_MENU, GAME_LOADING,GAME_WAIT_NETWORK, GAME_PLAYING, GAME_PAUSED, GAME_EDITOR };

	typedef struct  
	{
		bool forward, backward, left, right;
		int mouseDX, mouseDY;
	} NetworkPlayer;

	typedef struct  
	{
		float x, y, z;
		float rx, ry, rz;
	} PlayerSpawnPoint;

public:
	Engine();
	Engine(const Engine&);
	~Engine();

	bool Initialize();
	bool LoadConfiguration();
	bool InitializeGame();

	bool InitializeNetworking();
	bool ConnectNetworking();
	bool NewNetworkPlayer();
	bool CreateLocalPlayer();

	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Update();
	bool Render();
	bool PostUpdate();

	void PrepareToExit();

	void UpdateEntities(float dt, float loopCompletion);

	bool CreateEntityFromFile(
		char* filename, float x, float y, float z,
		float rx, float ry, float rz,
		float vx, float vy, float vz,
		bool isWeapon, int slot);

	bool SpawnPlayer(PlayerEntity* player, PlayerSpawnPoint spawnpoint);


	void InitializeWindows(int&, int&);
	void ShutdownWindows();

	PlayerEntity* Player() { return m_players[m_playerNumber]; };

	// Networking
	void MovePlayer(NetworkedInput inp, PlayerEntity* player, float dt);

private:

	// TESTING
	EntityModel *test;
	//
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputController* m_Input;
	GraphicsController* m_Graphics;

	PhysicsEntity* m_playerBoat;
	PhysicsEntity* m_otherBoat;

	PlayerEntity* m_players[MAXPLAYERS];
	PhysicsEntity* m_entities[MAXENTITIES];

	int m_numPlayers;

	int m_entityIndex;

	ProceduralTerrain* m_waterTerrain;
	Terrain* m_landTerrain;

	Bitmap* m_menuBitmap;
	Bitmap* m_networkLoadingBitmap;
	Bitmap* m_menuCursor;

	Bitmap* m_crosshair;

	float m_timeloopCompletion;

	float m_dt;

	NetworkServer* m_server;
	NetworkClient* m_client;

	bool m_connectedToServer;

	NetworkSyncController* m_networkSyncController;

	bool m_isServer;
	char m_serverAddress[16];

	GameState m_gameState;
	
	SYSTEMTIME newtime, oldtime;

	// Loading variables

	char m_levelFilename[256];
	char m_terrainMapFilename[256];
	char m_landTextureFilename[256];
	char m_skyboxTextureFilename[256];

	PlayerSpawnPoint m_spawnPoints[NUMBEROFTEAMS][MAXSPAWNPOINTS];
	int m_spawnPointIndex[NUMBEROFTEAMS];

	PhysicsEntity *m_weaponEntities[MAXWEAPONS];
	bool		  m_weapons[MAXWEAPONS];
	int			  m_currentWeapon;

	int m_playerTeam;
	int m_playerNumber;


#if EDITOR_BUILD

	PhysicsEntity* m_editCursor;

	bool m_editedSomething;

#endif // #if EDITOR_BUILD

	int m_screenHeight, m_screenWidth;

};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static Engine* ApplicationHandle = 0;