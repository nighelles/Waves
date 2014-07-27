#include "Engine.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	Engine* MainEngine;
	bool result;

	MainEngine = new Engine;
	if (!MainEngine)
	{
		return 0;
	}

	result = MainEngine->Initialize();
	if (result)
	{
		MainEngine->Run();
	}

	MainEngine->Shutdown();
	delete MainEngine;
	MainEngine = 0;

	return 0;
}