#include "Physics.h"


void PHY_SetupTick(
	PhysicsEntity* en,
	Terrain* land,
	ProceduralTerrain* water, float loopCompletion)
{
	float x, y, z;
	en->GetLocation(x, y, z);
	if (y < water->CalculateDeterministicHeight(x, z, loopCompletion))
		en->m_underwater = true;
	else
		en->m_underwater = false;


	en->m_grounded = false;
	if (y < land->CalculateDeterministicHeight(x, z, 0))
	{
		en->m_grounded = true;
	}
}

void PHY_EndTick(
	PhysicsEntity* en,
	Terrain* land,
	ProceduralTerrain* water, float loopCompletion, float dt)
{
	// move the thing

	if (en->m_impulseX != 0.0f)
	{
		en->m_velocityX += en->m_impulseX;
		en->m_impulseX = 0.0f;
	}
	if (en->m_impulseY != 0.0f)
	{
		en->m_velocityY += en->m_impulseY;
		en->m_impulseY = 0.0f;
	}
	if (en->m_impulseZ != 0.0f)
	{
		en->m_velocityZ += en->m_impulseZ;
		en->m_impulseZ = 0.0f;
	}

	// Check for intersections

	en->ApplyTranslation(en->m_velocityX*dt, en->m_velocityY*dt, en->m_velocityZ*dt);

	float x, y, z;
	en->GetLocation(x, y, z);
	float height = land->CalculateDeterministicHeight(x, z, 0);
	if (y < height)
	{
		D3DXVECTOR3 vel = en->GetVelocity();
		en->SetVelocity(vel.x, 0, vel.z);
		en->SetLocation(x, height, z);
		en->m_grounded = true;
	}

}

void PHY_ApplyGravity(PhysicsEntity* en, float dt)
{
	if (en->m_underwater)
	{
		en->ApplyImpulseAccel(0.0f, en->Bouyancy(), 0.0f, dt);
	}
	en->ApplyImpulseAccel(0.0f, -METERS(G_CONST), 0.0f, dt);
}

void PHY_Ground(PhysicsEntity* en, Terrain* ground)
{
	// FIX THIS
	float x, y, z;
	D3DXVECTOR3 preCollisionVelocity;

	en->GetLocation(x, y, z);
	preCollisionVelocity = en->GetVelocity();

	en->m_grounded = false;

	if (y < ground->CalculateDeterministicHeight(x,z,0))
	{
		float height = ground->CalculateDeterministicHeight(x,z,0);
		en->SetLocation(NULL, height, NULL);
		preCollisionVelocity.y = preCollisionVelocity.y / 10;
		if (preCollisionVelocity.y < 0.1) preCollisionVelocity.y = 0;
		en->SetVelocity(preCollisionVelocity.x, -preCollisionVelocity.y, preCollisionVelocity.z);
		en->m_grounded = true;
	}
	return;
}

float PHY_GetRayIntersection(EntityModel* model, D3DXVECTOR3* orig, D3DXVECTOR3* indir, D3DXVECTOR3* point)
{
	return PHY_GetRayIntersection(model, orig, indir, point, NULL);
}

float PHY_GetRayIntersection(EntityModel* model, D3DXVECTOR3* orig, D3DXVECTOR3* indir, D3DXVECTOR3* point, D3DXVECTOR3* normal)
{
	// I'm going to comment this, the t returned is DISTANCE TO THE POINT OF INTERSECTION
	// u,v is coordinates within triangle
	double t, u, v;
	bool intersected;

	D3DXVECTOR3 v0, v1, v2, dir, pos;

	D3DXVec3Normalize(&dir, indir);

	for (int i = 0; i != model->uniqueFaceCount; i += 1)
	{
		v0.x = model->uniqueVertices[model->m_modelDesc[i].vIndex1].x;
		v0.y = model->uniqueVertices[model->m_modelDesc[i].vIndex1].y;
		v0.z = model->uniqueVertices[model->m_modelDesc[i].vIndex1].z;

		v1.x = model->uniqueVertices[model->m_modelDesc[i].vIndex2].x;
		v1.y = model->uniqueVertices[model->m_modelDesc[i].vIndex2].y;
		v1.z = model->uniqueVertices[model->m_modelDesc[i].vIndex2].z;

		v2.x = model->uniqueVertices[model->m_modelDesc[i].vIndex3].x;
		v2.y = model->uniqueVertices[model->m_modelDesc[i].vIndex3].y;
		v2.z = model->uniqueVertices[model->m_modelDesc[i].vIndex3].z;

		D3DXVECTOR3 intPoint;

		intersected = intersect_triangle(orig, &dir, &v0, &v1, &v2, t, u, v, &intPoint);

		if (intersected)
		{
			// This only returns whatever the 1st vertex is, so it's sketchy, interpolation would be good.

			point->x = intPoint.x;
			point->y = intPoint.y;
			point->z = intPoint.z;

			if (normal)
			{
				normal->x += model->uniqueNormals[model->m_modelDesc[i].nIndex1].x;
				normal->y += model->uniqueNormals[model->m_modelDesc[i].nIndex1].y;
				normal->z += model->uniqueNormals[model->m_modelDesc[i].nIndex1].z;
				normal->x += model->uniqueNormals[model->m_modelDesc[i].nIndex2].x;
				normal->y += model->uniqueNormals[model->m_modelDesc[i].nIndex2].y;
				normal->z += model->uniqueNormals[model->m_modelDesc[i].nIndex2].z;
				normal->x += model->uniqueNormals[model->m_modelDesc[i].nIndex3].x;
				normal->y += model->uniqueNormals[model->m_modelDesc[i].nIndex3].y;
				normal->z += model->uniqueNormals[model->m_modelDesc[i].nIndex3].z;
				D3DXVec3Normalize(normal,normal);
			}
			return t;
		}
	}

	return -1;
}

