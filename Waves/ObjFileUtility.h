#pragma once

#include <iostream>
#include <fstream>
#include "EntityModel.h"

class ObjFileUtility
{
private:
	typedef struct
	{
		float x, y, z;
	} Vertex;

	typedef struct
	{
		int vIndex1, vIndex2, vIndex3;
		int tIndex1, tIndex2, tIndex3;
		int nIndex1, nIndex2, nIndex3;
	} Face;

public:
	ObjFileUtility();
	~ObjFileUtility();

	int	 loadNumberOfVertices(char* filename);

	bool LoadObjFile(char*, EntityModel::Model*);
	
	void Shutdown();

private:
	bool AnalizeFile(char* filename);

	Vertex *vertices, *texcoords, *normals;
	Face *faces;

	int m_vertexCount;
	int m_textureCount;
	int m_normalCount;
	int m_faceCount;
};

