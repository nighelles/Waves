#pragma once

#include <d3d11.h>
#include <d3dx10math.h>

class Entity
{
public:
	Entity();
	~Entity();

	virtual bool Initialize();
	void Shutdown();

	virtual void Update();

	void GetCameraLocation(float&, float&, float&);
	void GetBindLocation(float&, float&, float&);

	void SetRotation(float x, float y, float z);
	void SetLocation(float x, float y, float z);

	virtual void ApplyTranslation(float x, float y, float z);
	virtual void ApplyRotation(float x, float y, float z);

	void ApplyTranslationRelative(float x, float y, float z);

	void GetLocation(float&, float&, float&);
	void GetRotation(float&, float&, float&);

	virtual void BindToEntity(Entity* entity);
	virtual void UnbindFromEntity();

	bool Binded() const { return m_binded; }

protected:

	Entity* m_bindedEntity;
	bool	m_binded;

	float m_locationX;
	float m_locationY;
	float m_locationZ;

	float m_rotationX;
	float m_rotationY;
	float m_rotationZ;

	float m_cameraX;
	float m_cameraY;
	float m_cameraZ;

	float m_width;
	float m_height;
	float m_depth;
};

