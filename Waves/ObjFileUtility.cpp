#include "ObjFileUtility.h"

using namespace std;

ObjFileUtility::ObjFileUtility()
{
	m_vertexCount = 0;
	m_textureCount = 0;
	m_normalCount = 0;
	m_faceCount = 0;
	m_numMats = 0;
}


ObjFileUtility::~ObjFileUtility()
{
}

bool ObjFileUtility::LoadObjFile(char* filename, char* outputFilename,
								EntityModel::UniqueVertex *vertices, 
								EntityModel::UniqueVertex *texcoords, 
								EntityModel::UniqueVertex *normals, 
								EntityModel::UniqueFace *faces,
								EntityModel::SubModel *subModels)
{
	ifstream fin;
	ofstream fout;
	int vertexIndex, texcoordIndex, normalIndex, faceIndex;
	char input2;

	char input[256];

	char garbage[10];
	char textureFilename[256];
	char matName[256];

	int textureIndex = 0;

	vertexIndex = 0;
	texcoordIndex = 0;
	normalIndex = 0;
	faceIndex = 0;

	fin.open(filename);
	fout.open(outputFilename);

	if (fin.fail()) return false;
	if (fout.fail()) return false;

	subModels = new EntityModel::SubModel[m_numMats];
	int subModelIndex = 0;

	fout << m_numMats << endl;

	// REMEMBER TO CONVERT FROM LEFT HANDED COORDINATE SYSTEM
	fin >> input;
	while (!fin.eof())
	{
		if (strcmp(input, "v") == 0)
		{

			fin >> vertices[vertexIndex].x >> vertices[vertexIndex].y >> vertices[vertexIndex].z;

			vertices[vertexIndex].z = vertices[vertexIndex].z * -1.0f;
			vertexIndex++;
		}

		if (strcmp(input, "vt") == 0)
		{
			fin >> texcoords[texcoordIndex].x >> texcoords[texcoordIndex].y;

			texcoords[texcoordIndex].y = 1.0f - texcoords[texcoordIndex].y;
			texcoordIndex++;
		}

		if (strcmp(input, "vn") == 0)
		{
			fin >> normals[normalIndex].x >> normals[normalIndex].y >> normals[normalIndex].z;

			normals[normalIndex].z = normals[normalIndex].z * -1.0f;
			normalIndex++;
		}

		if (strcmp(input, "f") == 0)
		{

			fin >> faces[faceIndex].vIndex3 >> input2 >> faces[faceIndex].tIndex3 >> input2 >> faces[faceIndex].nIndex3
				>> faces[faceIndex].vIndex2 >> input2 >> faces[faceIndex].tIndex2 >> input2 >> faces[faceIndex].nIndex2
				>> faces[faceIndex].vIndex1 >> input2 >> faces[faceIndex].tIndex1 >> input2 >> faces[faceIndex].nIndex1;
				faceIndex++;

		}

		if (strcmp(input, "usemtl") == 0)
		{

			if (subModelIndex == 0)
			{
				fin >> matName;
				subModels[subModelIndex].begin = faceIndex;
				subModelIndex += 1;
			}
			else
			{
				subModels[subModelIndex-1].end = faceIndex;

				fout << "Material: " << matName << " " <<
					subModels[subModelIndex-1].begin << " " <<
					subModels[subModelIndex-1].end << endl;
			
				subModels[subModelIndex].begin = faceIndex;
				subModelIndex += 1;
				fin >> matName;
			}
		}

		fin >> input;
	}

	subModels[subModelIndex - 1].end = faceIndex;

	fout << "Material: " << matName << ".dds" << " " <<
		subModels[subModelIndex - 1].begin << " " <<
		subModels[subModelIndex - 1].end << endl;

	fin.close();
	fout.close();

	return true;
}

void ObjFileUtility::Shutdown()
{

	return;
}

bool ObjFileUtility::loadStats(char* filename, 
								int& numVertices, 
								int& numTexcoords, 
								int& numNormals, 
								int& numFaces,
								int& numMats)
{
	bool result;

	result = AnalizeFile(filename);
	if (!result) return 0;

	numVertices = m_vertexCount;
	numTexcoords = m_textureCount;
	numNormals = m_normalCount;
	numFaces = m_faceCount;
	numMats = m_numMats;

	return true;
}


bool ObjFileUtility::AnalizeFile(char* filename)
{
	ifstream fin;
	char input[256];
	
	m_vertexCount = 0;
	m_textureCount = 0;
	m_normalCount = 0;
	m_faceCount = 0;

	fin.open(filename);
	
	if (fin.fail() == true) return false;

	fin >> input;
	while (!fin.eof())
	{
		if (strcmp(input, "v") == 0) m_vertexCount++;
		if (strcmp(input, "vt") == 0) m_textureCount++;
		if (strcmp(input, "vn") == 0) m_normalCount++;
		if (strcmp(input, "f") == 0) m_faceCount++;
		if (strcmp(input, "usemtl") == 0) m_numMats += 1;;

		fin >> input;
	}

	fin.close();

	return true;
}