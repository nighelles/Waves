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

	// Initialize the indexes.
	vertexIndex = 0;
	texcoordIndex = 0;
	normalIndex = 0;
	faceIndex = 0;

	// Open the file.
	fin.open(filename);

	// Check if it was successful in opening the file.
	if (fin.fail() == true)
	{
		return false;
	}

	// Read in the vertices, texture coordinates, and normals into the data structures.
	// Important: Also convert to left hand coordinate system since Maya uses right hand coordinate system.
	fin.get(input);
	while (!fin.eof())
	{
		if (input == 'v')
		{
			fin.get(input);

			// Read in the vertices.
			if (input == ' ')
			{
				fin >> vertices[vertexIndex].x >> vertices[vertexIndex].y >> vertices[vertexIndex].z;

				// Invert the Z vertex to change to left hand system.
				vertices[vertexIndex].z = vertices[vertexIndex].z * -1.0f;
				vertexIndex++;
			}

			// Read in the texture uv coordinates.
			if (input == 't')
			{
				fin >> texcoords[texcoordIndex].x >> texcoords[texcoordIndex].y;

				// Invert the V texture coordinates to left hand system.
				texcoords[texcoordIndex].y = 1.0f - texcoords[texcoordIndex].y;
				texcoordIndex++;
			}

			// Read in the normals.
			if (input == 'n')
			{
				fin >> normals[normalIndex].x >> normals[normalIndex].y >> normals[normalIndex].z;

				// Invert the Z normal to change to left hand system.
				normals[normalIndex].z = normals[normalIndex].z * -1.0f;
				normalIndex++;
			}
		}

		// Read in the faces.
		if (input == 'f')
		{
			fin.get(input);
			if (input == ' ')
			{
				// Read the face data in backwards to convert it to a left hand system from right hand system.
				fin >> faces[faceIndex].vIndex3 >> input2 >> faces[faceIndex].tIndex3 >> input2 >> faces[faceIndex].nIndex3
					>> faces[faceIndex].vIndex2 >> input2 >> faces[faceIndex].tIndex2 >> input2 >> faces[faceIndex].nIndex2
					>> faces[faceIndex].vIndex1 >> input2 >> faces[faceIndex].tIndex1 >> input2 >> faces[faceIndex].nIndex1;
				faceIndex++;
			}
		}

		// Read in the remainder of the line.
		while (input != '\n')
		{
			fin.get(input);
		}

		// Start reading the beginning of the next line.
		fin.get(input);
	}

	// Close the file.
	fin.close();

	// Now loop through all the faces and output the three vertices for each face.
	for (int i = 0; i!=m_faceCount; ++i)
	{
		vIndex = faces[i].vIndex1 - 1;
		tIndex = faces[i].tIndex1 - 1;
		nIndex = faces[i].nIndex1 - 1;

		modelData[3 * i].x = vertices[vIndex].x;
		modelData[3 * i].y = vertices[vIndex].y;
		modelData[3 * i].z = vertices[vIndex].z;

		modelData[3 * i].tu = texcoords[vIndex].x;
		modelData[3 * i].tv = texcoords[vIndex].y;

		modelData[3 * i].nx = normals[vIndex].x;
		modelData[3 * i].ny = normals[vIndex].y;
		modelData[3 * i].nz = normals[vIndex].z;

		vIndex = faces[i].vIndex2 - 1;
		tIndex = faces[i].tIndex2 - 1;
		nIndex = faces[i].nIndex2 - 1;

		modelData[3*i + 1].x = vertices[vIndex].x;
		modelData[3*i + 1].y = vertices[vIndex].y;
		modelData[3*i + 1].z = vertices[vIndex].z;

		modelData[3 * i + 1].tu = texcoords[vIndex].x;
		modelData[3 * i + 1].tv = texcoords[vIndex].y;

		modelData[3 * i + 1].nx = normals[vIndex].x;
		modelData[3 * i + 1].ny = normals[vIndex].y;
		modelData[3 * i + 1].nz = normals[vIndex].z;

		vIndex = faces[i].vIndex3 - 1;
		tIndex = faces[i].tIndex3 - 1;
		nIndex = faces[i].nIndex3 - 1;

		modelData[3*i + 2].x = vertices[vIndex].x;
		modelData[3*i + 2].y = vertices[vIndex].y;
		modelData[3*i + 2].z = vertices[vIndex].z;

		modelData[3 * i + 2].tu = texcoords[vIndex].x;
		modelData[3 * i + 2].tv = texcoords[vIndex].y;

		modelData[3 * i + 2].nx = normals[vIndex].x;
		modelData[3 * i + 2].ny = normals[vIndex].y;
		modelData[3 * i + 2].nz = normals[vIndex].z;
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