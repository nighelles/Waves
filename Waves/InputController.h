#pragma once

#define DIRECTINPUT_VERSION 0x0800

#include <dinput.h>

class InputController
{
public:
	InputController();
	InputController(const InputController&);
	~InputController();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	bool IsEscapePressed();

	void IsMouseClicked(bool& left, bool& right);
	void GetMouseLocation(int&, int&);
	void GetMouseDelta(int&, int&);

	bool IsKeyPressed(int key);
	bool IsKeyDown(int key);
	bool IsKeyUp(int key);

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

private:
	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;

	unsigned char m_keyboardState[256];
	unsigned char m_keyboardStateOld[256];
	DIMOUSESTATE m_mouseState;

	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;
};
