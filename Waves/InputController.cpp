#include "InputController.h"

InputController::InputController()
{
}


InputController::InputController(const InputController& other)
{
}


InputController::~InputController()
{
}


void InputController::Initialize()
{
	int i;

	for (i = 0; i<256; i++)
	{
		m_keys[i] = false;
	}

	return;
}


void InputController::KeyDown(unsigned int input)
{
	m_keys[input] = true;
	return;
}


void InputController::KeyUp(unsigned int input)
{
	m_keys[input] = false;
	return;
}


bool InputController::IsKeyDown(unsigned int key)
{
	return m_keys[key];
}