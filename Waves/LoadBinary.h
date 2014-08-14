#pragma once

#include <iostream>
#include <fstream>
#include <EntityModel.h>

class LoadBinary
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
	LoadBinary();
	~LoadBinary();

	bool loadStats(char* filename, int& numFrames, int& numVertices, int& numNormals, int& numTexcoords);

	bool loadBinaryFile(char* filename, EntityModel::Unique)
};