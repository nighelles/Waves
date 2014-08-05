#include "raytriangle.h"

bool intersect_triangle(D3DXVECTOR3* orig, D3DXVECTOR3* dir, D3DXVECTOR3* vert0, D3DXVECTOR3* vert1, D3DXVECTOR3* vert2, double& t, double& u, double& v)
{
	D3DXVECTOR3 edge1, edge2, tvec, pvec, qvec;
	double det, inv_det;

	D3DXVec3Subtract(&edge1, vert1, vert0);
	D3DXVec3Subtract(&edge2, vert2, vert0);

	D3DXVec3Cross(&pvec, dir, &edge2);

	det = D3DXVec3Dot(&edge1, &pvec);

	if (det < EPSILON) return false;

	D3DXVec3Subtract(&tvec, orig, vert0);

	u = D3DXVec3Dot(&tvec, &pvec);

	if (u<0.0 || u>det) return false;

	D3DXVec3Cross(&qvec, &tvec, &edge1);

	v = D3DXVec3Dot(dir, &qvec);
	if (v < 0.0 || u + v > det) return false;

	t = D3DXVec3Dot(&edge2, &qvec);
	inv_det = 1.0 / det;

	t *= inv_det;
	u *= inv_det;
	v *= inv_det;

	return true;
}

