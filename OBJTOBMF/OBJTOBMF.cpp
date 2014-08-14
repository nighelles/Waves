// OBJTOBMF.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <tchar.h>
#include <iostream>

#include "EntityModel.h"
#include "ObjFileUtility.h"

using namespace std;

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

char filename[20];
char newfilename[20];

int numFrames;

EntityModel::UniqueFace	  *modelDesc;
EntityModel::UniqueVertex *uniqueVertices;
EntityModel::UniqueVertex *uniqueNormals;
EntityModel::UniqueVertex *uniqueTexcoords;

int numVerts, numNormals, numTexcoords, numFaces;


bool writeBinaryFile(char* filename);

int _tmain(int argc, _TCHAR* argv[])
{
	cout << "Filename to convert: ";
	cin >> filename;
	cout << "Number of frames: ";
	cin >> numFrames;
	/////

	ObjFileUtility* objLoader;
	objLoader = new ObjFileUtility;

	for (int i = 0; i < numFrames; ++i)
	{
		sprintf(newfilename, "%s%d.obj", filename, i);

		printf("%s\n", newfilename);

		if (i == 0)
		{
			// first file, load stats
			objLoader->loadStats(newfilename, numVerts, numTexcoords, numNormals, numFaces);

			modelDesc = new EntityModel::UniqueFace[numFaces];
			uniqueVertices = new EntityModel::UniqueVertex[numVerts*numFrames];
			uniqueNormals = new EntityModel::UniqueVertex[numNormals*numFrames];
			uniqueTexcoords = new EntityModel::UniqueVertex[numTexcoords*numFrames];
		}

		objLoader->LoadObjFile(newfilename, 
			&(uniqueVertices[i*numVerts]),
			&(uniqueTexcoords[i*numTexcoords]),
			&(uniqueNormals[i*numNormals]),
			modelDesc);
	}

	sprintf(newfilename, "%s.bmf", filename);

	writeBinaryFile(newfilename);
	cout << "Done." << endl;

	objLoader->Shutdown(); delete objLoader; objLoader = 0;
	delete[] modelDesc;
	delete[] uniqueVertices;
	delete[] uniqueNormals;
	delete[] uniqueTexcoords;

	return 0;
}

bool writeBinaryFile(char* filename)
{
	FILE *file = NULL;
	if ((file = fopen(filename, "wb")) == NULL) return false;

	fwrite(&numFrames, sizeof(int), 1, file);
	fwrite(&numFaces, sizeof(int), 1, file);
	fwrite(&numVerts, sizeof(int), 1, file);
	fwrite(&numNormals, sizeof(int), 1, file);
	fwrite(&numTexcoords, sizeof(int), 1, file);
	fwrite(modelDesc, sizeof(int), numFaces * 9, file);
	fwrite(uniqueVertices, sizeof(float), numVerts * 3 * numFrames, file);
	fwrite(uniqueNormals, sizeof(float), numNormals * 3 * numFrames, file);
	fwrite(uniqueTexcoords, sizeof(float), numTexcoords * 3 * numFrames, file);

	fclose(file);

	return true;
}