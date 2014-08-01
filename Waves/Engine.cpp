#include "Engine.h"

Engine::Engine()
{
	m_Input = 0;
	m_Graphics = 0;

	m_playerBoat = 0;
	m_waterTerrain = 0;

	m_menuBitmap = 0;

	m_gameState = GAME_MENU;
}

Engine::Engine(const Engine& other)
{
}

Engine::~Engine()
{
}

bool Engine::Initialize()
{
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

	// Initialize a boat model
	m_playerBoat = new PhysicsEntity;

	result = m_playerBoat->Initialize();
	if (!result) return false;

	result = m_playerBoat->InitializeModel(m_Graphics, "Boat.obj", L"wood_tiling.dds");
	if (!result) return false;

	// Attach Camera to boat model
	m_Graphics->GetPlayerCamera()->BindToEntity(m_playerBoat);

	// Deal with creating water
	m_waterTerrain = new ProceduralTerrain();
	if (!m_waterTerrain) return false;

	result = m_waterTerrain->Initialize(m_Graphics->GetRenderController()->GetDevice(), L"water_tiling.dds");
	if (!result)
	{
		MessageBox(m_hwnd, L"Could not Initialize Water Terrain", L"Error", MB_OK);
		return false;
	}
	m_waterTerrain->m_shaderType = EntityModel::WATER_SHADER;

	m_Graphics->RegisterEntityModel(m_waterTerrain);

	// GAME_STATE

	m_menuBitmap->SetVisible(false);

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
		m_Input = 0;
	}
	if (m_waterTerrain)
	{
		m_waterTerrain->Shutdown();
		delete m_waterTerrain;
		m_waterTerrain = 0;
	}
	if (m_menuBitmap)
	{
		m_menuBitmap->Shutdown();
		delete m_menuBitmap;
		m_menuBitmap = 0;
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
					InitializeGame();
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

bool Engine::Update()
{
	bool result;
	float playerCameraX, playerCameraY, playerCameraZ;
	int deltaT;
	SYSTEMTIME sysTime;
	
	GetSystemTime(&sysTime);
	m_oldTime = m_Time;
	m_Time = sysTime.wSecond * 1000 + sysTime.wMilliseconds;

	if (m_Input->IsKeyPressed(DIK_W))
		m_playerBoat->ApplyTranslationRelative(0.0f, 0.0f, 1.0f);
	if (m_Input->IsKeyPressed(DIK_A))
		m_playerBoat->ApplyRotation(0.f, -1.0f, 0.0f);
	if (m_Input->IsKeyPressed(DIK_S))
		m_playerBoat->ApplyTranslationRelative(0.0f, 0.0f, -1.0f);
	if (m_Input->IsKeyPressed(DIK_D))
		m_playerBoat->ApplyRotation(0.f, 1.0f, 0.0f);

	// CALCULATE BOAT HEIGHT
	D3DXVECTOR3 a, b, c, d;
	float fr, fl, br, bl;
	float roll, pitch;

	roll = 0; pitch = 0;

	float x, y, z;
	m_playerBoat->GetLocation(x, y, z);
	m_playerBoat->GetBoundingBox(a, b, c, d);

	fr = m_waterTerrain->CalculateDeterministicHeight(b.x, b.z, m_Time / 60000.0f);
	fl = m_waterTerrain->CalculateDeterministicHeight(a.x, a.z, m_Time / 60000.0f);
	bl = m_waterTerrain->CalculateDeterministicHeight(d.x, d.z, m_Time / 60000.0f);
	br = m_waterTerrain->CalculateDeterministicHeight(c.x, c.z, m_Time / 60000.0f);

	roll = -180.0f/3.14f*atan2(((fl + bl) - (fr + br)) / 2.0, 3); // fix absolue 3 as width
	m_playerBoat->SetRotation(NULL, NULL, roll);

	pitch = -180.0f/3.14f*atan2(((fl + fr) - (bl + br)) / 2.0, 5);
	m_playerBoat->SetRotation(pitch, NULL, NULL);


	m_playerBoat->SetLocation(NULL, m_waterTerrain->CalculateDeterministicHeight(x, z, m_Time / 60000.0f) + 1, NULL);

	m_playerBoat->Update(m_Graphics);

	m_playerBoat->GetCameraLocation(playerCameraX, playerCameraY, playerCameraZ);
	//m_Graphics->GetPlayerCamera()->SetLocation(playerCameraX,playerCameraY,playerCameraZ);

	m_Graphics->GetPlayerCamera()->Update();

	// END CALCULATE BOAT HEIGHT

	return true;
}

bool Engine::Render()
{
	// Render the current frame
	int mouseX, mouseY;
	int mouseDX, mouseDY;
	bool result;
	
	m_Input->GetMouseLocation(mouseX, mouseY);
	m_Input->GetMouseDelta(mouseDX, mouseDY);
	

	// This will have to be changed once the player is just a physics object
	// Camera should just use the physics object stuff I wrote.

	if (m_gameState == GAME_PLAYING)
	{
		result = m_Graphics->Frame(mouseX, mouseY, mouseDX, mouseDY, m_Time / 60000.0f);
	}
	else if (m_gameState == GAME_MENU)
	{
		result = m_Graphics->Frame(0, 0, 0, 0, m_Time / 60000.0f);
	}
	if (!result) return false;

	result = m_Graphics->Render();
	if (!result) return false;

	return true;
}

bool Engine::PostUpdate()
{
	// Anything that needs to happen after the frame has rendered
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