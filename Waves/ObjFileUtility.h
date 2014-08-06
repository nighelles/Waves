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

	bool loadStats(char* filename, int& numVertices, int& numTexcoords, int& numNormals, int& numFaces);

	bool LoadObjFile(char* filename, EntityModel::UniqueVertex *vertices, EntityModel::UniqueVertex *texcoords, EntityModel::UniqueVertex *normals, EntityModel::UniqueFace *faces);
	
	void Shutdown();

private:
	bool AnalizeFile(char* filename);

	int m_vertexCount;
	int m_textureCount;
	int m_normalCount;
	int m_faceCount;
};

