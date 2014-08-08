#pragma once

#include "PhysicsEntity.h"
#include "raytriangle.h"
#include "PhysicsDefs.h"

#include "ProceduralTerrain.h"

void PHY_ApplyGravity(PhysicsEntity* en, float dt);

bool PHY_GetRayIntersection(EntityModel* model, D3DXVECTOR3* orig, D3DXVECTOR3* indir, D3DXVECTOR3* point);
bool PHY_GetRayIntersection(EntityModel* model, D3DXVECTOR3* orig, D3DXVECTOR3* indir, D3DXVECTOR3* point, D3DXVECTOR3* normal);

void PHY_SetupTick(PhysicsEntity* en, Terrain* land, ProceduralTerrain* water, float loopCompletion);
void PHY_EndTick(PhysicsEntity* en, Terrain* land, ProceduralTerrain* water, float loopCompletion);