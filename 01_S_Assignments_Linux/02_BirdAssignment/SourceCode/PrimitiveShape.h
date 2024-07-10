#ifndef _PRIMITIVESHAPE_H
#define _PRIMITIVESHAPE_H

class PrimitiveShape
{
public:
	float *vertices;
	float *normals;
	float *texCoords;
	unsigned short *indices;

	int verticesSize;
	int normalsSize;
	int texCoordsSize;
	int indicesSize;
	PrimitiveShape(int slices, int stacks)
	{
		int vertexCount = (slices + 1)*(stacks + 1);
		vertices = (float*)malloc(3 * vertexCount * sizeof(float));
		normals = (float *)malloc(3 * vertexCount * sizeof(float));
		texCoords = (float*)malloc(2 * vertexCount * sizeof(float));
		indices = (unsigned short *)malloc((2 * slices*(stacks) * 3 * sizeof(unsigned short)));

		verticesSize= 3 * vertexCount;
		normalsSize= 3 * vertexCount;
		texCoordsSize= 2 * vertexCount;
		indicesSize= 2 * slices*(stacks) * 3;
	}

	PrimitiveShape(int verticesSize, int normalsSize,int texCoordsSize,int indicesSize)
	{
		vertices = (float*)malloc(verticesSize * sizeof(float));
		normals = (float *)malloc(normalsSize * sizeof(float));
		texCoords = (float*)malloc(texCoordsSize * sizeof(float));
		indices = (unsigned short *)malloc(indicesSize * sizeof(unsigned short));

		this->verticesSize = verticesSize;
		this->normalsSize = normalsSize;
		this->texCoordsSize = texCoordsSize;
		this->indicesSize = indicesSize;
	}

	void ClearPrimitiveShape()
	{
		if (vertices)
		{
			free(vertices);
			vertices = NULL;
		}
		if (normals)
		{
			free(normals);
			normals=NULL;
		}
		if (texCoords)
		{
			free(texCoords);
			texCoords = NULL;
		}
		if (normals)
		{
			free(normals);
			normals = NULL;
		}
		if (indices)
		{
			free(indices);
			indices = NULL;
		}
	}
};
#endif 
