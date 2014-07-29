#pragma once

#include <windows.h>

#include "InputController.h"
#include "GraphicsController.h"
#include "Entity.h"

class Engine
{
public:
	Engine();
	Engine(const Engine&);
	~Engine();

	bool Initialize();
	void Shutdown();
	void Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);

private:
	bool Update();
	void InitializeWindows(int&, int&);
	void ShutdownWindows();

private:
	LPCWSTR m_applicationName;
	HINSTANCE m_hinstance;
	HWND m_hwnd;

	InputController* m_Input;
	GraphicsController* m_Graphics;

	Entity* m_playerBoat;

	int m_Time;
	int m_oldTime;
};

static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static Engine* ApplicationHandle = 0;