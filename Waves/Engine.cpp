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
	m_otherPlayer = 0;
#endif

#if EDITOR_BUILD
	m_editCursor = 0;
#endif

	m_waterTerrain = 0;
	m_landTerrain = 0;

	m_menuBitmap = 0;
	m_networkLoadingBitmap = 0;

	m_crosshair = 0;

	m_server = 0;
	m_client = 0;

	m_networkSyncController = 0;
	m_networkPlayer = { };

	m_gameState = GAME_MENU;

	m_playerConnected = false;
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

	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Start with menu
	m_gameState = GAME_MENU;

	m_menuBitmap = new Bitmap;
	if (!m_menuBitmap) return false;

	result = m_menuBitmap->Initialize(m_Graphics->GetRenderController()->GetDevice(), screenWidth, screenHeight, L"menu.dds", screenWidth, screenHeight);
	if (!result) return false;
	m_menuBitmap->UpdateBuffers(m_Graphics->GetRenderController()->GetDeviceContext(), 0, 0);
	m_menuBitmap->SetVisible(true);
	m_Graphics->RegisterBitmap(m_menuBitmap);

	// crosshair for game

	m_crosshair = new Bitmap;
	if (!m_crosshair) return false;
	result = m_crosshair->Initialize(m_Graphics->GetRenderController()->GetDevice(), screenWidth, screenHeight, L"crosshair.dds", 32, 32);
	if (!result) return false;
	m_crosshair->SetVisible(true);
	m_Graphics->RegisterBitmap(m_crosshair);

	// setup loading screen for network

	m_networkLoadingBitmap = new Bitmap;
	if (!m_networkLoadingBitmap) return false;

	result = m_networkLoadingBitmap->Initialize(m_Graphics->GetRenderController()->GetDevice(), screenWidth, screenHeight, L"networkloading.dds", screenWidth, screenHeight);
	if (!result) return false;

	m_networkLoadingBitmap->SetVisible(false);

	m_Graphics->RegisterBitmap(m_networkLoadingBitmap);

	return true;
}

bool Engine::InitializeGame()
{
	bool result;

	// Change these to the new Register model structuring

	// TESTING

	test = new EntityModel;

	test->loadBinaryFile("glock.bmf");
	test->Initialize(m_Graphics->GetRenderController()->GetDevice(), L"metal.dds");
	test->SetLocation(100, 5, 100);

	test->CurrentFrame(4);

	m_Graphics->RegisterEntityModel(test);

	// END TESTING

#if GAME_BUILD
	// Initialize our player

	m_player = new PlayerEntity;

	result = m_player->InitializeModel(m_Graphics, "player.obj", L"cursor.dds");
	if (!result) return false;
	result = m_player->Initialize();
	if (!result) return false;

	m_player->SetLocation(METERS(100.0f),METERS(10.0f),METERS(100.0f));
	m_player->Render(m_Graphics);

	// other network player

	m_otherPlayer = new PlayerEntity;
	
	result = m_otherPlayer->InitializeModel(m_Graphics, "player.obj", L"cursor.dds");
	if (!result) return false;
	result = m_otherPlayer->Initialize();
	if (!result) return false;

	m_otherPlayer->SetLocation(0.0f, METERS(5.0f), 0.0f);
	m_otherPlayer->Render(m_Graphics);

	// Initialize a boat model
	m_playerBoat = new PhysicsEntity;

	result = m_playerBoat->InitializeModel(m_Graphics, "Boat.obj", L"wood_tiling.dds");
	if (!result) return false;
	result = m_playerBoat->Initialize();
	if (!result) return false;


	// Initialize other person's boat
	m_otherBoat = new PhysicsEntity;
	result = m_otherBoat->InitializeModel(m_Graphics, "Boat.obj", L"wood_tiling.dds");
	if (!result) return false;
	result = m_otherBoat->Initialize();
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

	// Deal with creating water
	m_waterTerrain = new ProceduralTerrain();
	if (!m_waterTerrain) return false;

	result = m_waterTerrain->Initialize(m_Graphics->GetRenderController()->GetDevice(), L"water_trans.dds");
	if (!result)
	{
		OutputDebugString(L"Could not Initialize Water Terrain");
		return false;
	}
	m_waterTerrain->m_shaderType = EntityModel::WATER_SHADER;

	m_Graphics->RegisterEntityModel(m_waterTerrain);

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

	m_crosshair->UpdateBuffers(m_Graphics->GetRenderController()->GetDeviceContext(), m_screenWidth / 2 - 16, m_screenHeight / 2 - 16);

	return true;
}

#if USE_NETWORKING
bool Engine::InitializeNetworking()
{
	m_networkLoadingBitmap->SetVisible(true);

	m_networkSyncController = new NetworkSyncController;

	if (m_isServer)
	{
		m_server = new NetworkServer;
		m_server->Initialize();
	}
	else {
		m_client = new NetworkClient;
		m_client->Initialize();
	}

	return true;
}

bool Engine::ConnectNetworking()
{
	if (m_isServer)
	{
		if (!m_server->WaitForClient()) 
			return false;

		m_networkSyncController->Initialize(m_isServer, m_server);
	}
	else {
		if (!m_client->ConnectToServer(m_serverAddress)) 
			return false;

		m_networkSyncController->Initialize(m_isServer, m_client);
	}

	if (m_server) {
		m_networkSyncController->RegisterEntity(m_player); //server
		m_networkSyncController->RegisterEntity(m_otherPlayer); // not server
		m_player->SetLocation(METERS(-100), 0, METERS(-100));
		m_otherPlayer->SetLocation(METERS(100), 0, METERS(100));
	}
	else 
	{
		m_networkSyncController->RegisterEntity(m_otherPlayer); // server
		m_networkSyncController->RegisterEntity(m_player); // notserver
		m_otherPlayer->SetLocation(METERS(-100), 0, METERS(-100));
		m_player->SetLocation(METERS(100), 0, METERS(100));
	}

	// set later after we debug position
	//m_Graphics->GetEntityModel(m_player->ModelID())->IsVisible(false);

	m_networkSyncController->RegisterEntity(m_playerBoat);

	// done loading
	m_networkLoadingBitmap->SetVisible(false);
	return true;
}

#endif //#if USE_NETWORKING

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
	if (m_otherPlayer)
	{
		m_otherPlayer->Shutdown();
		delete m_otherPlayer;
		m_otherPlayer = 0;
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
	if (m_crosshair)
	{
		m_crosshair->Shutdown();
		delete m_crosshair;
		m_crosshair = 0;
	}
	if (m_networkLoadingBitmap)
	{
		m_networkLoadingBitmap->Shutdown();
		delete m_networkLoadingBitmap;
		m_networkLoadingBitmap = 0;
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
	int mouseX, mouseY;

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
				m_Input->GetMouseLocation(mouseX, mouseY);
				m_crosshair->UpdateBuffers(m_Graphics->GetRenderController()->GetDeviceContext(), mouseX, mouseY);
				if (mouse1)
				{
					if (!InitializeGame())
					{
						done = true;
						break;
					}
					m_gameState = GAME_PLAYING;
#if USE_NETWORKING
					m_gameState = GAME_WAIT_NETWORK;
					InitializeNetworking();
#endif
				}

				result = Render();
				if (!result) done = true;
			}
#if USE_NETWORKING
			else if (m_gameState == GAME_WAIT_NETWORK)
			{
				if (ConnectNetworking()) m_gameState = GAME_PLAYING;

				result = Render();
				if (!result) done = true;
			}
#endif
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

	int mouseIDX, mouseIDY;
	int mouseX, mouseY;
	float mouseDX, mouseDY;

	oldtime = newtime;
	GetSystemTime(&newtime);

	m_dt = newtime.wMilliseconds - oldtime.wMilliseconds;

	if (m_dt < 0) m_dt += 1000;
	m_dt /= 1000.0;

	m_timeloopCompletion = (newtime.wSecond * 1000 + newtime.wMilliseconds) / 60000.0;

	m_Input->GetMouseDelta(mouseIDX, mouseIDY);

	mouseDX = mouseIDX * m_dt * 10;
	mouseDY = mouseIDY * m_dt * 10;

#if GAME_BUILD

	if (m_Input->IsKeyPressed(DIK_T))
		test->Animating(true);

	NetworkedInput playerInput{ };

	if (m_Input->IsKeyPressed(DIK_W))
		playerInput.keys[Network_W] = true;
	if (m_Input->IsKeyPressed(DIK_A))
		playerInput.keys[Network_A] = true;
	if (m_Input->IsKeyPressed(DIK_S))
		playerInput.keys[Network_S] = true;
	if (m_Input->IsKeyPressed(DIK_D))
		playerInput.keys[Network_D] = true;
	if (m_Input->IsKeyPressed(DIK_SPACE))
		playerInput.keys[Network_SPACE] = true;
	if (m_Input->IsKeyPressed(DIK_LCONTROL))
		playerInput.keys[Network_CONTROL] = true;
	if (m_Input->IsKeyPressed(DIK_LSHIFT))
		playerInput.keys[Network_SHIFT] = true;

	if (m_Input->IsKeyDown(DIK_SPACE))
		playerInput.keys[Network_SPACE] = true;

	playerInput.mouseDX = mouseDX;
	playerInput.mouseDY = mouseDY;

	m_Graphics->GetPlayerCamera()->ApplyRotation(mouseDY, 0.0, 0.0);

	D3DXVECTOR3 rot = m_Graphics->GetPlayerCamera()->GetRotation();
	float px, py, pz;
	m_player->GetRotation(px, py, py);
	m_Graphics->GetPlayerCamera()->SetRotation(rot.x, py, rot.z);

	MovePlayer(playerInput, m_player, m_dt);

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

	UpdateEntities(m_dt, m_timeloopCompletion);

#if USE_NETWORKING

	if (m_isServer)
	{
		m_networkSyncController->SyncPlayerInput(&playerInput);

		MovePlayer(playerInput, m_otherPlayer, m_dt);
	}
	else 
	{
		m_networkSyncController->SyncPlayerInput(&playerInput);
	}

#endif //#if USE_NETWORKING


	return true;
}

void Engine::MovePlayer(NetworkedInput inp, PlayerEntity* player, float dt)
{
	D3DXVECTOR3 dir = D3DXVECTOR3(0, 0, 0);

	if (inp.keys[Network_W])
		dir.z += 1;
	if (inp.keys[Network_A])
		dir.x -= 1;
	if (inp.keys[Network_S])
		dir.z -= 1;
	if (inp.keys[Network_D])
		dir.x += 1;
	if (inp.keys[Network_SPACE])
		dir.y += 1;
	if (inp.keys[Network_CONTROL])
		dir.y -= 1;
	if (inp.keys[Network_SHIFT])
		player->Run(true);
	else
		player->Run(false);

	if (inp.keys[Network_SPACE])
	{
		player->Jump(dt);
	}

	if (dir.x == 0 && dir.y == 0 && dir.z == 0)
		player->Stop(dt);
	else
		player->Movement(dir.x, dir.y, dir.z, dt);


	player->ApplyRotation(0, inp.mouseDX, 0); // THIS SHOULD BE BASED ON DT ALSO
}

void Engine::UpdateEntities(float dt, float loopCompletion)
{

#if GAME_BUILD
	m_playerBoat->Tick(dt);
	m_otherBoat->Tick(dt);

	m_playerBoat->OrientToTerrain(m_waterTerrain, m_timeloopCompletion);
	m_otherBoat->OrientToTerrain(m_waterTerrain, m_timeloopCompletion);

	m_player->Tick(dt);

	PHY_SetupTick(m_player, m_landTerrain, m_waterTerrain, m_timeloopCompletion);

	// This is just if the entity is affected by water, probably should fix that
	if (m_player->m_underwater) {
		m_Graphics->GetLight()->SetDiffuseColor(0.2f, 0.2f, 1.0f, 1.0f);
	}
	else {
		m_Graphics->GetLight()->SetDiffuseColor(1.0f,1.0f,1.0f, 1.0f);
	}

	PHY_ApplyGravity(m_player, dt);
	
	PHY_EndTick(m_player, m_landTerrain, m_waterTerrain, m_timeloopCompletion,dt);

	m_playerBoat->Render(m_Graphics);
	m_otherBoat->Render(m_Graphics);
	m_player->Render(m_Graphics);
#endif // #if GAME_BUILD

#if USE_NETWORKING
	m_otherPlayer->Tick(dt);
	PHY_SetupTick(m_otherPlayer, m_landTerrain, m_waterTerrain, m_timeloopCompletion);
	PHY_ApplyGravity(m_otherPlayer, dt);
	PHY_EndTick(m_otherPlayer, m_landTerrain, m_waterTerrain, m_timeloopCompletion, dt);
	m_otherPlayer->Render(m_Graphics);
#endif

#if EDITOR_BUILD
	m_editCursor->Render(m_Graphics);
#endif

	m_Graphics->GetPlayerCamera()->Update();
	return;
}

bool Engine::Render()
{
	// Render the current frame
	bool result;

	result = m_Graphics->Frame(m_timeloopCompletion);

	if (!result) return false;

	result = m_Graphics->Render(m_dt);
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