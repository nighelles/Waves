#include "Entity.h"


Entity::Entity()
{
	m_locationX = 0.0f;
	m_locationY = 0.0f;
	m_locationZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;

	m_modelID = 0;
}


Entity::~Entity()
{
}

bool Entity::Initialize() {
	return true;
}

bool Entity::InitializeModel(GraphicsController* graphics, char* modelFilename, WCHAR* textureFilename)
{
	bool result;

	result = graphics->InitializeEntityModel(modelFilename, textureFilename);
	if (!result)
		return false;
	else
		m_modelID = result;

	return true;
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