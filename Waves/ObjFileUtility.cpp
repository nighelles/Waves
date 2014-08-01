#include "ObjFileUtility.h"

using namespace std;

ObjFileUtility::ObjFileUtility()
{
	int m_vertexCount = 0;
	int m_textureCount = 0;
	int m_normalCount = 0;
	int m_faceCount = 0;

	vertices = 0;
	texcoords = 0;
	normals = 0;

	faces = 0;
}


ObjFileUtility::~ObjFileUtility()
{
}

bool ObjFileUtility::LoadObjFile(char* filename, EntityModel::Model *modelData)
{
	ifstream fin;
	int vertexIndex, texcoordIndex, normalIndex, faceIndex, vIndex, tIndex, nIndex;
	char input, input2;
	ofstream fout;


	// Initialize the four data structures.
	vertices = new Vertex[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	texcoords = new Vertex[m_textureCount];
	if (!texcoords)
	{
		return false;
	}

	normals = new Vertex[m_normalCount];
	if (!normals)
	{
		return false;
	}

	faces = new Face[m_faceCount];
	if (!faces)
	{
		return false;
	}

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

	for (int i = 0; i!=m_faceCount; ++i)
	{
		vIndex = faces[i].vIndex1 - 1;
		tIndex = faces[i].tIndex1 - 1;
		nIndex = faces[i].nIndex1 - 1;

		modelData[3 * i].x = vertices[vIndex].x;
		modelData[3 * i].y = vertices[vIndex].y;
		modelData[3 * i].z = vertices[vIndex].z;

		modelData[3 * i].tu = texcoords[tIndex].x;
		modelData[3 * i].tv = texcoords[tIndex].y;

		modelData[3 * i].nx = normals[nIndex].x;
		modelData[3 * i].ny = normals[nIndex].y;
		modelData[3 * i].nz = normals[nIndex].z;

		vIndex = faces[i].vIndex2 - 1;
		tIndex = faces[i].tIndex2 - 1;
		nIndex = faces[i].nIndex2 - 1;

		modelData[3*i + 1].x = vertices[vIndex].x;
		modelData[3*i + 1].y = vertices[vIndex].y;
		modelData[3*i + 1].z = vertices[vIndex].z;

		modelData[3 * i + 1].tu = texcoords[tIndex].x;
		modelData[3 * i + 1].tv = texcoords[tIndex].y;

		modelData[3 * i + 1].nx = normals[nIndex].x;
		modelData[3 * i + 1].ny = normals[nIndex].y;
		modelData[3 * i + 1].nz = normals[nIndex].z;

		vIndex = faces[i].vIndex3 - 1;
		tIndex = faces[i].tIndex3 - 1;
		nIndex = faces[i].nIndex3 - 1;

		modelData[3*i + 2].x = vertices[vIndex].x;
		modelData[3*i + 2].y = vertices[vIndex].y;
		modelData[3*i + 2].z = vertices[vIndex].z;

		modelData[3 * i + 2].tu = texcoords[tIndex].x;
		modelData[3 * i + 2].tv = texcoords[tIndex].y;

		modelData[3 * i + 2].nx = normals[nIndex].x;
		modelData[3 * i + 2].ny = normals[nIndex].y;
		modelData[3 * i + 2].nz = normals[nIndex].z;
	}

	return true;
}

void ObjFileUtility::Shutdown()
{
	if (vertices)
	{
		delete[] vertices;
		vertices = 0;
	}
	if (texcoords)
	{
		delete[] texcoords;
		texcoords = 0;
	}
	if (normals)
	{
		delete[] normals;
		normals = 0;
	}
	if (faces)
	{
		delete[] faces;
		faces = 0;
	}
	return;
}

int ObjFileUtility::loadNumberOfVertices(char* filename)
{
	bool result;

	result = AnalizeFile(filename);
	if (!result) return 0;

	return m_faceCount * 3;
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