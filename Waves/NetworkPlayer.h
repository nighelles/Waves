#pragma once
class NetworkPlayer
{
public:
	NetworkPlayer();
	~NetworkPlayer();

private:
	bool forward, backward, right, left;

	bool Left() const { return left; }
	void Left(bool val) { left = val; }
	bool Right() const { return right; }
	void Right(bool val) { right = val; }
	bool Backward() const { return backward; }
	void Backward(bool val) { backward = val; }
	bool Forward() const { return forward; }
	void Forward(bool val) { forward = val; }

	float mouseDX, mouseDY;

	float MouseDY() const { return mouseDY; }
	void MouseDY(float val) { mouseDY = val; }
	float MouseDX() const { return mouseDX; }
	void MouseDX(float val) { mouseDX = val; }
};

