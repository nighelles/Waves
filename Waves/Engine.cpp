#include "Engine.h"

#include <atldef.h>
#include <atlstr.h>

Engine::Engine()
{
	m_Input = 0;
	m_Graphics = 0;

	m_playerBoat = 0;
	m_otherBoat = 0;

#if GAME_BUILD
	m_player = 0;
#endif

#if EDITOR_BUILD
	m_editCursor = 0;
#endif

	m_waterTerrain = 0;
	m_landTerrain = 0;

	m_menuBitmap = 0;

	m_server = 0;
	m_client = 0;

	m_networkSyncController = 0;
	m_networkPlayer = { };

	m_gameState = GAME_MENU;
}

Engine::Engine(const Engine& other)
{
}

Engine::~Engine()
{
}

bool Engine::LoadConfiguration()
{
	ifstream fin;
	char input;

	fin.open("config.txt");
	if (fin.fail()) return false;

	fin.get(input);
	while (input != ':') fin.get(input);
	fin >> m_isServer;
	fin.get(input);
	while (input != ':') fin.get(input);
	fin >> m_serverAddress;
	fin.get(input);
	while (input != ':') fin.get(input);
	fin >> m_terrainMapFilename;

	if (m_isServer) 
		OutputDebugString(L"Server\n");
	else 
		OutputDebugString(L"Not Server\n");

	OutputDebugString(L"Server Address: ");
	OutputDebugString(ATL::CA2W(m_serverAddress));
	OutputDebugString(L"\n");

	return true;
}

bool Engine::Initialize()
{
	LoadConfiguration();

	// GAMESTATE
	m_gameState = GAME_LOADING;

	int screenWidth, screenHeight;
	bool result;

	screenWidth = 0;
	screenHeight = 0;

	InitializeWindows(screenWidth, screenHeight);

	m_Input = new InputController;
	if (!m_Input)
	{
		return false;
	}

	result = m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(m_hwnd, L"Could not initialize DirectInput", L"Error", MB_OK);
		return false;
	}

	m_Graphics = new GraphicsController;
	if (!m_Graphics)
	{
		return false;
	}

	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result) return false;

	// Start with menu
	m_gameState = GAME_MENU;

	m_menuBitmap = new Bitmap;
	if (!m_menuBitmap) return false;

	result = m_menuBitmap->Initialize(m_Graphics->GetRenderController()->GetDevice(), screenWidth, screenHeight, L"menu.dds", screenWidth, screenHeight);
	if (!result) return false;

	m_menuBitmap->SetVisible(true);

	m_Graphics->RegisterBitmap(m_menuBitmap);

	return true;
}

bool Engine::InitializeGame()
{
	bool result;

	// Change these to the new Register model structuring

#if GAME_BUILD
	// Initialize our player
	m_player = new PlayerEntity;
	result = m_player->Initialize();
	if (!result) return false;
	result = m_player->InitializeModel(m_Graphics, "player.obj",L"cursor.dds");
	if (!result) return false;

	m_player->SetLocation(0.0f,METERS(5.0f),0.0f);
	m_player->Render(m_Graphics);

	// Initialize a boat model
	m_playerBoat = new PhysicsEntity;

	result = m_playerBoat->Initialize();
	if (!result) return false;

	result = m_playerBoat->InitializeModel(m_Graphics, "Boat.obj", L"wood_tiling.dds");
	if (!result) return false;

	// Initialize other person's boat
	m_otherBoat = new PhysicsEntity;
	result = m_otherBoat->Initialize();
	if (!result) return false;
	result = m_otherBoat->InitializeModel(m_Graphics, "Boat.obj", L"wood_tiling.dds");
	if (!result) return false;

	// Put things in place
	m_playerBoat->SetLocation(100.0f, 0, 0);
	m_playerBoat->Render(m_Graphics);

	m_otherBoat->SetLocation(100.0f, 0, 0);
	m_otherBoat->Render(m_Graphics);

	// Attach Camera to boat model
	m_Graphics->GetPlayerCamera()->BindToEntity(m_player);

	m_Graphics->GetPlayerCamera()->Update();
	m_Graphics->GetPlayerCamera()->Render();
#endif //#if GAME_BUILD

#if EDITOR_BUILD
	m_Graphics->GetPlayerCamera()->SetLocation(0.0f,METERS(100.0f),0.0f);
	m_editCursor = new PhysicsEntity;
	result = m_editCursor->Initialize();
	if (!result) return false;
	result = m_editCursor->InitializeModel(m_Graphics, "EditorCursor.obj", L"cursor.dds");
#endif //#if EDITOR_BUILD

	// Deal with creating water
	m_waterTerrain = new ProceduralTerrain();
	if (!m_waterTerrain) return false;

	result = m_waterTerrain->Initialize(m_Graphics->GetRenderController()->GetDevice(), L"water_tiling.dds");
	if (!result)
	{
		OutputDebugString(L"Could not Initialize Water Terrain");
		return false;
	}
	m_waterTerrain->m_shaderType = EntityModel::WATER_SHADER;

	m_Graphics->RegisterEntityModel(m_waterTerrain);

	m_landTerrain = new Terrain();
	if (!m_landTerrain) return false;

	result = m_landTerrain->Initialize(m_Graphics->GetRenderController()->GetDevice(), L"sand_tiling.dds");
	if (!result)
	{
		OutputDebugString(L"Could not Initialize Sand Terrain");
		return false;
	}
	m_landTerrain->m_shaderType = EntityModel::TEXTURE_SHADER;

	m_Graphics->RegisterEntityModel(m_landTerrain);

	result = m_landTerrain->LoadTerrainMap(m_terrainMapFilename);

#if GAME_BUILD
	if (!result)
	{
		OutputDebugString(L"Couldn't load map!\n");
		return false;
	}
#endif // #if GAME_BUILD

#if EDITOR_BUILD
	if (!result)
	{
		OutputDebugString(L"Creating new map.\n");
	}
#endif // #if EDITOR_BUILD

	// GAME_STATE

	m_menuBitmap->SetVisible(false);

	// NETWORKING INITIALIZATION

#if USE_NETWORKING
	m_networkSyncController = new NetworkSyncController;
	
	if (m_isServer)
	{
		m_server = new NetworkServer;
		m_server->Initialize();

		m_server->WaitForClient();
		
		m_networkSyncController->Initialize(m_isServer, m_server);
	}
	else {
		m_client = new NetworkClient;
		m_client->Initialize();

		m_client->ConnectToServer(m_serverAddress);

		m_networkSyncController->Initialize(m_isServer, m_client);
	}

	m_networkSyncController->RegisterEntity(m_playerBoat);
	m_networkSyncController->RegisterEntity(m_otherBoat);
	m_networkSyncController->RegisterEntity(m_island);

#endif // #if USE_NETWORKING

	return true;
}

void Engine::Shutdown()
{
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}
	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}
	if (m_playerBoat)
	{
		m_playerBoat->Shutdown();
		delete m_playerBoat;
		m_playerBoat = 0;
	}
	if (m_player)
	{
		m_player->Shutdown();
		delete m_player;
		m_player = 0;
	}
	if (m_otherBoat)
	{
		m_otherBoat->Shutdown();
		delete m_otherBoat;
		m_otherBoat = 0;
	}
#if EDITOR_BUILD
	if (m_editCursor)
	{
		m_editCursor->Shutdown();
		delete m_editCursor;
		m_editCursor = 0;
	}
#endif
	if (m_waterTerrain)
	{
		m_waterTerrain->Shutdown();
		delete m_waterTerrain;
		m_waterTerrain = 0;
	}
	if (m_landTerrain)
	{
		m_landTerrain->Shutdown();
		delete m_landTerrain;
		m_landTerrain = 0;
	}
	if (m_menuBitmap)
	{
		m_menuBitmap->Shutdown();
		delete m_menuBitmap;
		m_menuBitmap = 0;
	}
	if (m_client)
	{
		m_client->Shutdown();
		delete m_client;
		m_client = 0;
	}
	if (m_server)
	{
		m_server->Shutdown();
		delete m_server;
		m_server = 0;
	}
	if (m_networkSyncController)
	{
		m_networkSyncController->Shutdown();
		delete m_networkSyncController;
		m_networkSyncController = 0;
	}
	ShutdownWindows();

	return;
}

void Engine::Run()
{
	MSG msg;
	bool done, result;
	bool mouse1,mouse2;

	ZeroMemory(&msg, sizeof(MSG));

	done = false;
	while (!done)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (msg.message == WM_QUIT || m_Input->IsEscapePressed())
		{
			PrepareToExit();
			done = true;
		}
		else
		{
			result = m_Input->Frame();
			if (!result) done = true;

			m_Input->IsMouseClicked(mouse1, mouse2);
			if (m_gameState == GAME_MENU)
			{
				if (mouse1)
				{
					if (!InitializeGame())
					{
						done = true;
						break;
					}
					m_gameState = GAME_PLAYING;
				}

				result = Render();
				if (!result) done = true;
			}
			else if (m_gameState == GAME_PLAYING)
			{
				// Main game logic
				result = Update();
				if (!result) done = true;
				result = Render();
				if (!result) done = true;
				result = PostUpdate();
				if (!result) done = true;
			}
		}

	}

	return;

}

void Engine::PrepareToExit()
{
#if EDITOR_BUILD
	if (m_editedSomething)
	{
		OutputDebugString(L"Saving Map\n");
		m_landTerrain->SaveTerrainMap(m_terrainMapFilename);
	}
#endif

	return;
}

bool Engine::Update()
{
	bool result;

	int mouseX, mouseY;
	int mouseDX, mouseDY;
	
	oldtime = newtime;
	GetSystemTime(&newtime);
	
	float dt = newtime.wMilliseconds - oldtime.wMilliseconds;

	if (dt < 0) dt += 1000;
	dt /= 1000.0;

	oldtime = newtime;

	m_Time = newtime.wSecond * 1000 + newtime.wMilliseconds;


	m_Input->GetMouseDelta(mouseDX, mouseDY);

#if GAME_BUILD

	D3DXVECTOR3 dir = D3DXVECTOR3(0, 0, 0);

	if (m_Input->IsKeyPressed(DIK_W))
		dir.z += 1;
	if (m_Input->IsKeyPressed(DIK_A))
		dir.x -= 1;
	if (m_Input->IsKeyPressed(DIK_S))
		dir.z -= 1;
	if (m_Input->IsKeyPressed(DIK_D))
		dir.x += 1;
	
	D3DXVec3Normalize(&dir, &dir);
	
	if (m_Input->IsKeyDown(DIK_SPACE))
		m_player->Jump();


	if (dir.x == 0 && dir.y == 0 && dir.z == 0)
		m_player->Stop(dt);
	else
		m_player->Movement(dir.x, dir.y, dir.z, dt);
	

	m_player->ApplyRotation(0,mouseDX,0); // THIS SHOULD BE BASED ON DT ALSO
	m_Graphics->GetPlayerCamera()->ApplyRotation(mouseDY,mouseDX,0.0);

#endif // #if GAME_BUILD

#if EDITOR_BUILD
	m_Graphics->GetPlayerCamera()->ApplyRotation(mouseDY, mouseDX, 0.0);

	if (m_Input->IsKeyPressed(DIK_W))
		m_Graphics->GetPlayerCamera()->ApplyTranslationRelative(0.0f, 0.0f, 5.0f);
	if (m_Input->IsKeyPressed(DIK_A))
		m_Graphics->GetPlayerCamera()->ApplyTranslationRelative(-5.0f, 0.0f, 0.0f);
	if (m_Input->IsKeyPressed(DIK_S))
		m_Graphics->GetPlayerCamera()->ApplyTranslationRelative(0.0f, 0.0f, -5.0f);
	if (m_Input->IsKeyPressed(DIK_D))
		m_Graphics->GetPlayerCamera()->ApplyTranslationRelative(5.0f, 0.0f, 0.0f);
	if (m_Input->IsKeyDown(DIK_F))
		m_waterTerrain->IsVisible(!m_waterTerrain->IsVisible());
	
	D3DXVECTOR3 editorRayOrigin, editorRayDirection, editorRayIntersection;
	bool editorRayIntersected;

	editorRayOrigin = m_Graphics->GetPlayerCamera()->GetPosition();
	editorRayDirection = m_Graphics->GetPlayerCamera()->GetDirection();

	editorRayIntersected = PHY_GetRayIntersection(m_landTerrain, &editorRayOrigin, &editorRayDirection, &editorRayIntersection);

	if (editorRayIntersected)
	{
		float cursorY = editorRayIntersection.y;
		if (cursorY < 5.0f && m_waterTerrain->IsVisible()) cursorY = 5.0f;

		m_editCursor->SetLocation(editorRayIntersection.x, cursorY, editorRayIntersection.z);
		
		if (m_Input->IsKeyPressed(DIK_Q))
		{
			m_landTerrain->ApplyVerticalOffset(editorRayIntersection.x, editorRayIntersection.z, 20.0, -5.0);
			m_editedSomething = true;
		}
		if (m_Input->IsKeyPressed(DIK_E))
		{
			m_landTerrain->ApplyVerticalOffset(editorRayIntersection.x, editorRayIntersection.z, 20.0, 5.0);
			m_editedSomething = true;
		}
		if (m_Input->IsKeyPressed(DIK_R))
		{
			m_landTerrain->SetVerticalOffset(editorRayIntersection.x, editorRayIntersection.z, 20.0, 5.0f);
			m_editedSomething = true;
		}
	}

#endif

	UpdateEntities(dt);

#if USE_NETWORKING

	bool forward,backward,left,right;
	int mouseDX,mouseDY;

	if(!m_isServer)
	{
		forward = m_Input->IsKeyPressed(DIK_W);
		backward = m_Input->IsKeyPressed(DIK_S);
		left = m_Input->IsKeyPressed(DIK_A);
		right = m_Input->IsKeyPressed(DIK_D);
		m_Input->GetMouseDelta(mouseDX, mouseDY);
	}

	m_networkSyncController->SyncPlayerInput(forward, backward, left, right, mouseDX, mouseDY);

	if (m_isServer)
	{
		m_networkPlayer.forward = forward;
		m_networkPlayer.backward = backward;
		m_networkPlayer.left = left;
		m_networkPlayer.right = right;
		m_networkPlayer.mouseDX = mouseDX;
		m_networkPlayer.mouseDY = mouseDY;
	}


	if (m_networkPlayer.forward)
		m_otherBoat->ApplyImpulse(0.0f, 0.0f, 1.0f);
	if (m_networkPlayer.left)
		m_playerBoat->ApplyRotation(0.f, -1.0f, 0.0f);
	if (m_networkPlayer.backward)
		m_playerBoat->ApplyImpulse(0.0f, 0.0f, -1.0f);
	if (m_networkPlayer.right)
		m_playerBoat->ApplyRotation(0.f, 1.0f, 0.0f);

#endif //#if USE_NETWORKING


	return true;
}

void Engine::UpdateEntities(float dt)
{
	m_Graphics->GetPlayerCamera()->Update();

#if GAME_BUILD
	m_playerBoat->Tick(dt);
	m_otherBoat->Tick(dt);

	m_playerBoat->OrientToTerrain(m_waterTerrain, m_Time / 60000.0);
	m_otherBoat->OrientToTerrain(m_waterTerrain, m_Time / 60000.0);

	m_player->Tick(dt);

	PHY_ApplyGravity(m_player, dt);
	PHY_Ground(m_player, m_landTerrain);

	m_playerBoat->Render(m_Graphics);
	m_otherBoat->Render(m_Graphics);
	m_player->Render(m_Graphics);

#endif // #if GAME_BUILD

#if EDITOR_BUILD
	m_editCursor->Update(m_Graphics, dt);
#endif

	return;
}

bool Engine::Render()
{
	// Render the current frame
	bool result;

	result = m_Graphics->Frame(m_Time / 60000.0f);

	if (!result) return false;

	result = m_Graphics->Render();
	if (!result) return false;

	return true;
}

bool Engine::PostUpdate()
{
	// Anything that needs to happen after the frame has rendered

#if USE_NETWORKING

		m_networkSyncController->SyncEntityStates();

#endif //#if USE_NETWORKING

	return true;
}


LRESULT CALLBACK Engine::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

void Engine::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	ApplicationHandle = this;

	m_hinstance = GetModuleHandle(NULL);

	m_applicationName = L"Waves";

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	RegisterClassEx(&wc);

	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	if (FULL_SCREEN)
	{
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		posX = posY = 0;
	}
	else
	{
		screenWidth = 800;
		screenHeight = 600;

		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	ShowCursor(false);

	return;
}

void Engine::ShutdownWindows()
{
	ShowCursor(true);

	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}