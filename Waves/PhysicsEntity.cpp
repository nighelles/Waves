#include "PhysicsEntity.h"


PhysicsEntity::PhysicsEntity() : Entity()
{
	m_modelID = 0;

	m_velocityX = 0.0f;
	m_velocityY = 0.0f;
	m_velocityZ = 0.0f;

	m_velocityDamping = 0.0f;

	m_grounded = false;

	m_bouyancy = METERS(G_CONST);

	m_entityModel = 0;

	type = PHYSICS;
}


PhysicsEntity::~PhysicsEntity()
{
}

bool PhysicsEntity::InitializeModel(GraphicsController* graphics, char* modelFilename, char* textureFilename)
{
	bool result;

	m_entityModel = new EntityModel;
	
	result = m_entityModel->loadBinaryFile(modelFilename);
	if (!result)
	{
		OutputDebugString(L"Could not load binary file");
		return false;
	}
	
	result = m_entityModel->Initialize(graphics->GetRenderController()->GetDevice(), textureFilename);
	if (!result)
	{
		OutputDebugString(L"Could not initialize entity model.");
		return false;
	}

	m_modelID = graphics->RegisterEntityModel(m_entityModel);
	
	if (m_modelID == -1)
		return false;

	return true;
}

void PhysicsEntity::Tick(float dt)
{
	// moved to EndTick in Physics.cpp

	Entity::Update(); // should override
}

void PhysicsEntity::Render(GraphicsController* graphics)
{
	graphics->GetEntityModel(m_modelID)->SetLocation(m_locationX, m_locationY, m_locationZ);
	graphics->GetEntityModel(m_modelID)->SetRotation(m_rotationX, m_rotationY, m_rotationZ);
}

// DEAL WITH THIS LATER?
void PhysicsEntity::GetBoundingBox(D3DXVECTOR3& a, D3DXVECTOR3& b, D3DXVECTOR3& c, D3DXVECTOR3& d)
{
	// Code reuse :(
	D3DXMATRIX rotationMatrix;
	float yaw, pitch, roll;

	yaw = m_rotationY * 0.0174532925f;
	pitch = m_rotationX * 0.0174532925f;
	roll = m_rotationZ * 0.0174532925f;

	D3DXMatrixRotationYawPitchRoll(&rotationMatrix, yaw, 0.0f, 0.0f);

	a.x = -m_width / 2.0f;
	a.y = 0.0f;
	a.z = m_depth / 2.0f;

	b.x = m_width / 2.0f;
	b.y = 0.0f;
	b.z = m_depth / 2.0f;

	c.x = m_width / 2.0f;
	c.y = 0.0f;
	c.z = -m_depth / 2.0f;

	d.x = -m_width / 2.0f;
	d.y = 0.0f;
	d.z = -m_depth / 2.0f;

	D3DXVec3TransformCoord(&a, &a, &rotationMatrix);
	D3DXVec3TransformCoord(&b, &b, &rotationMatrix);
	D3DXVec3TransformCoord(&c, &c, &rotationMatrix);
	D3DXVec3TransformCoord(&d, &d, &rotationMatrix);

	a.x += m_locationX;
	a.y += m_locationY;
	a.z += m_locationZ;
	b.x += m_locationX;
	b.y += m_locationY;
	b.z += m_locationZ;
	c.x += m_locationX;
	c.y += m_locationY;
	c.z += m_locationZ;
	d.x += m_locationX;
	d.y += m_locationY;
	d.z += m_locationZ;

	return;
}

void PhysicsEntity::OrientToTerrain(ProceduralTerrain* terrain, float time)
{
	D3DXVECTOR3 a, b, c, d;
	float fr, fl, br, bl;
	float roll, pitch;

	roll = 0; pitch = 0;

	float x, y, z;
	GetLocation(x, y, z);
	GetBoundingBox(a, b, c, d);

	fr = terrain->CalculateDeterministicHeight(b.x, b.z, time);
	fl = terrain->CalculateDeterministicHeight(a.x, a.z, time);
	bl = terrain->CalculateDeterministicHeight(d.x, d.z, time);
	br = terrain->CalculateDeterministicHeight(c.x, c.z, time);

	roll = -180.0f / 3.14f*atan2(((fl + bl) - (fr + br)) / 2.0, 3); // fix absolue 3 as width
	SetRotation(NULL, NULL, roll);

	pitch = -180.0f / 3.14f*atan2(((fl + fr) - (bl + br)) / 2.0, 5);
	SetRotation(pitch, NULL, NULL);


	SetLocation(NULL, terrain->CalculateDeterministicHeight(x, z, time) + 1, NULL);

	return;
}

void PhysicsEntity::ApplyImpulseDirect(float x, float y, float z)
{
	if (x) m_impulseX += x;
	if (y) m_impulseY += y;
	if (z) m_impulseZ += z;
}

void PhysicsEntity::ApplyImpulseAccel(float x, float y, float z, float dt)
{
	if (x) m_impulseX += x*dt;
	if (y) m_impulseY += y*dt;
	if (z) m_impulseZ += z*dt;
}

void PhysicsEntity::SetVelocity(float x, float y, float z)
{
	m_velocityX = x;
	m_velocityY = y;
	m_velocityZ = z;
}

void PhysicsEntity::Shutdown()
{
	Entity::Shutdown();

	if (m_entityModel)
	{
		m_entityModel->Shutdown();
		delete m_entityModel;
		m_entityModel = 0;
	}
}
