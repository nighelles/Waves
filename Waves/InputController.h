#pragma once

class InputController
{
public:
	InputController();
	InputController(const InputController&);
	~InputController();

	void Initialize();

	void KeyDown(unsigned int);
	void KeyUp(unsigned int);

	bool IsKeyDown(unsigned int);

private:
	bool m_keys[256];
};
