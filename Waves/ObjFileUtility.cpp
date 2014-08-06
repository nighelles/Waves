#include "ObjFileUtility.h"

using namespace std;

ObjFileUtility::ObjFileUtility()
{
	int m_vertexCount = 0;
	int m_textureCount = 0;
	int m_normalCount = 0;
	int m_faceCount = 0;
}


ObjFileUtility::~ObjFileUtility()
{
}

bool ObjFileUtility::LoadObjFile(char* filename, EntityModel::UniqueVertex *vertices, EntityModel::UniqueVertex *texcoords, EntityModel::UniqueVertex *normals, EntityModel::UniqueFace *faces)
{
	ifstream fin;
	int vertexIndex, texcoordIndex, normalIndex, faceIndex, vIndex, tIndex, nIndex;
	char input, input2;
	ofstream fout;

	vertexIndex = 0;
	texcoordIndex = 0;
	normalIndex = 0;
	faceIndex = 0;

	fin.open(filename);

	if (fin.fail()) return false;


	// REMEMBER TO CONVERT FROM LEFT HANDED COORDINATE SYSTEM
	fin.get(input);
	while (!fin.eof())
	{
		if (input == 'v')
		{
			fin.get(input);

			if (input == ' ')
			{
				fin >> vertices[vertexIndex].x >> vertices[vertexIndex].y >> vertices[vertexIndex].z;

				vertices[vertexIndex].z = vertices[vertexIndex].z * -1.0f;
				vertexIndex++;
			}

			if (input == 't')
			{
				fin >> texcoords[texcoordIndex].x >> texcoords[texcoordIndex].y;

				texcoords[texcoordIndex].y = 1.0f - texcoords[texcoordIndex].y;
				texcoordIndex++;
			}

			if (input == 'n')
			{
				fin >> normals[normalIndex].x >> normals[normalIndex].y >> normals[normalIndex].z;

				normals[normalIndex].z = normals[normalIndex].z * -1.0f;
				normalIndex++;
			}
		}

		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ')
			{
				fin >> faces[faceIndex].vIndex3 >> input2 >> faces[faceIndex].tIndex3 >> input2 >> faces[faceIndex].nIndex3
					>> faces[faceIndex].vIndex2 >> input2 >> faces[faceIndex].tIndex2 >> input2 >> faces[faceIndex].nIndex2
					>> faces[faceIndex].vIndex1 >> input2 >> faces[faceIndex].tIndex1 >> input2 >> faces[faceIndex].nIndex1;
				faceIndex++;
			}
		}

		while (input != '\n')
		{
			fin.get(input);
		}

		fin.get(input);
	}

	fin.close();

	return true;
}

void ObjFileUtility::Shutdown()
{

	return;
}

bool ObjFileUtility::loadStats(char* filename, int& numVertices, int& numTexcoords, int& numNormals, int& numFaces)
{
	bool result;

	result = AnalizeFile(filename);
	if (!result) return 0;

	numVertices = m_vertexCount;
	numTexcoords = m_textureCount;
	numNormals = m_normalCount;
	numFaces = m_faceCount;

	return true;
}


bool ObjFileUtility::AnalizeFile(char* filename)
{
	ifstream fin;
	char input;
	
	m_vertexCount = 0;
	m_textureCount = 0;
	m_normalCount = 0;
	m_faceCount = 0;

	fin.open(filename);
	
	if (fin.fail() == true) return false;

	fin.get(input);
	while (!fin.eof())
	{
		if (input == 'v')
		{
			fin.get(input);
			if (input == ' ') { m_vertexCount++; }
			if (input == 't') { m_textureCount++; }
			if (input == 'n') { m_normalCount++; }
		}

		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ') { m_faceCount++; }
		}

		while (input != '\n')
		{
			fin.get(input);
		}

		fin.get(input);
	}

	fin.close();

	return true;
}