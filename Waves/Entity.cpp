#include "Entity.h"


Entity::Entity()
{
	m_locationX = 0.0f;
	m_locationY = 0.0f;
	m_locationZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;

	m_Model = 0;
}


Entity::~Entity()
{
}

bool Entity::Initialize() {
	bool result;

	m_Model = new EntityModel;
	if (!m_Model) return false;

	return true;
}

bool Entity::InitializeModel(ID3D11Device* device, WCHAR* textureFilename)
{
	bool result;

	result = m_Model->Initialize(device, L"water_tiling.dds");
	if (!result) return false;

	return true;
}

void Entity::Render(ID3D11DeviceContext* deviceContext)
{
	m_Model->Render(deviceContext);
}

void Entity::Shutdown()
{
	if (m_Model)
	{
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}
}

void Entity::Update()
{

}

EntityModel* Entity::GetModel()
{
	return m_Model;
}