#pragma once

// Based on the following paper 
// http://www.cs.virginia.edu/~gfx/Courses/2003/ImageSynthesis/papers/Acceleration/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf0

#include <d3d11.h>
#include <d3dx10math.h>
#include "EntityModel.h"

#define EPSILON 0.000001

bool intersect_triangle(D3DXVECTOR3* orig, D3DXVECTOR3* dir,
	D3DXVECTOR3* vert0,
	D3DXVECTOR3* vert1,
	D3DXVECTOR3* vert2,
	double& t, double& u, double& v, D3DXVECTOR3* intPoint);

