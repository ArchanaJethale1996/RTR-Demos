#include<math.h>
void drawSphere(float radius, int slices, int stacks, float *vertices, float *normals, float *texCoords, unsigned short *indices) {

	radius = radius > 0 ? radius : 0.5f;
	slices = slices > 0 ? slices : 32;
	stacks = stacks > 0 ? stacks : 16;
	int vertexCount = (slices + 1)*(stacks + 1);
	//float vertices[5000];				// = (float*)malloc(3 * vertexCount * sizeof(float));
	//float normals[5000];				// = (float *)malloc(3 * vertexCount * sizeof(float));
	//float texCoords[5000];				// = (float*)malloc(2 * vertexCount * sizeof(float));
	//unsigned short indices[5000];		// = (unsigned short *)malloc(2 * slices*stacks * 3 * sizeof(unsigned short));
	double du = 2 * M_PI / slices;
	double dv = M_PI / stacks;
	int i, j;
	double u, v, x, y, z;
	int indexV = 0;
	int indexT = 0;
	for (i = 0; i <= stacks; i++) {
		v = -M_PI / 2 + i*dv;
		for (j = 0; j <= slices; j++) {
			u = (j*du);
			x = (cosf(u)*cosf(v));
			y = (sinf(u)*cosf(v));
			z = sinf(v);
			vertices[indexV] = radius*x;
			normals[indexV++] = x;
			vertices[indexV] = radius*y;
			normals[indexV++] = y;
			vertices[indexV] = radius*z;
			normals[indexV++] = z;
			texCoords[indexT++] = j / slices;
			texCoords[indexT++] = i / stacks;
		}
	}
	int k = 0;
	for (j = 0; j < stacks; j++) {
		int row1 = j*(slices + 1);
		int row2 = (j + 1)*(slices + 1);
		for (i = 0; i < slices; i++) {
			indices[k++] = row1 + i;
			indices[k++] = row2 + i + 1;
			indices[k++] = row2 + i;
			indices[k++] = row1 + i;
			indices[k++] = row1 + i + 1;
			indices[k++] = row2 + i + 1;
		}
	}
}



void drawHemiSphere(float radius, int slices, int stacks, float *vertices, float *normals, float *texCoords, unsigned short *indices) {

	radius = radius > 0 ? radius : 0.5f;
	slices = slices > 0 ? slices : 32;
	stacks = stacks > 0 ? stacks : 16;
	int vertexCount = (slices + 1)*(stacks + 1);
	//float vertices[5000];				// = (float*)malloc(3 * vertexCount * sizeof(float));
	//float normals[5000];				// = (float *)malloc(3 * vertexCount * sizeof(float));
	//float texCoords[5000];				// = (float*)malloc(2 * vertexCount * sizeof(float));
	//unsigned short indices[5000];		// = (unsigned short *)malloc(2 * slices*stacks * 3 * sizeof(unsigned short));
	double du = 2 * M_PI / slices;
	double dv = M_PI / stacks;
	int i, j;
	double u, v, x, y, z;
	int indexV = 0;
	int indexT = 0;
	for (i = 0; i <= stacks/2; i++) {
		v = -M_PI / 2 + i*dv;
		for (j = 0; j <= slices; j++) {
			u = (j*du);
			x = (cosf(u)*cosf(v));
			y = (sinf(u)*cosf(v));
			z = sinf(v);
			vertices[indexV] = radius*x;
			normals[indexV++] = x;
			vertices[indexV] = radius*y;
			normals[indexV++] = y;
			vertices[indexV] = radius*z;
			normals[indexV++] = z;
			texCoords[indexT++] = j / slices;
			texCoords[indexT++] = i / stacks;
		}
	}
	int k = 0;
	for (j = 0; j < stacks; j++) {
		int row1 = j*(slices + 1);
		int row2 = (j + 1)*(slices + 1);
		for (i = 0; i < slices; i++) {
			indices[k++] = row1 + i;
			indices[k++] = row2 + i + 1;
			indices[k++] = row2 + i;
			indices[k++] = row1 + i;
			indices[k++] = row1 + i + 1;
			indices[k++] = row2 + i + 1;
		}
	}
}

void drawCylinder(double baseRadius, double topRadius,double height, int slices, int stacks, float *vertices, float *normals, float *texCoords, unsigned short *indices)
{
	int i, j;
	float angle;
	float zLow, zHigh;
	float length;
	float deltaRadius;
	float zNormal;
	float xyNormalRatio;
	float radiusLow;
		
	/* Compute length (needed for normal calculations) */
	deltaRadius = baseRadius - topRadius;
	length = sqrt(deltaRadius*deltaRadius + height*height);
	if (length == 0.0) {		
		return;
	}

	zNormal = deltaRadius / length;
	xyNormalRatio = height / length;
	int indexV = 0;
	int indexT = 0;
	for (i = 0; i <= slices; i++) {
		angle = 2 * M_PI * i / slices;
		for (j = 0; j <= stacks; j++) {
			zLow = j * height / stacks;
			radiusLow = baseRadius - deltaRadius * ((float)j / stacks);

			vertices[indexV] = radiusLow * sinf(angle);
			normals[indexV++] = xyNormalRatio * sinf(angle);
			vertices[indexV] = radiusLow * cosf(angle);
			normals[indexV++] = xyNormalRatio * cosf(angle);
			vertices[indexV] = zLow;
			normals[indexV++] = zNormal;
			texCoords[indexT++] = 1 - (float)i / slices;
			texCoords[indexT++] = (float)j / stacks;
		}
	}

	int k = 0;
	for (j = 0; j < stacks; j++) {
		int row1 = j*(slices + 1);
		int row2 = (j + 1)*(slices + 1);
		for (i = 0; i < slices; i++) {
			indices[k++] = row1 + i;
			indices[k++] = row2 + i + 1;
			indices[k++] = row2 + i;
			indices[k++] = row1 + i;
			indices[k++] = row1 + i + 1;
			indices[k++] = row2 + i + 1;
		}
	}
}

void drawPartialDisk(double innerRadius,double outerRadius, int slices, int loops,double startAngle, double sweepAngle,  float *vertices, float *normals, float *texCoords, unsigned short *indices)
{
	int i, j;
	float sinCache[5000];
	float cosCache[5000];
	float angle;
	float sintemp, costemp;
	float deltaRadius;
	float radiusLow, radiusHigh;
	float texLow = 0.0, texHigh = 0.0;
	float angleOffset;
	int slices2;
	int finish;


	if (sweepAngle < -360.0) sweepAngle = 360.0;
	if (sweepAngle > 360.0) sweepAngle = 360.0;
	if (sweepAngle < 0) {
		startAngle += sweepAngle;
		sweepAngle = -sweepAngle;
	}

	if (sweepAngle == 360.0) {
		slices2 = slices;
	}
	else {
		slices2 = slices + 1;
	}

	/* Compute length (needed for normal calculations) */
	deltaRadius = outerRadius - innerRadius;

	angleOffset = startAngle / 180.0 * M_PI;
	for (i = 0; i <= slices; i++) {
		angle = angleOffset + ((M_PI * sweepAngle) / 180.0) * i / slices;
		sinCache[i] = sinf(angle);
		cosCache[i] = cosf(angle);
	}

	if (sweepAngle == 360.0) {
		sinCache[slices] = sinCache[0];
		cosCache[slices] = cosCache[0];
	}

	int indexV = 0;
	int indexT = 0;

	for (i = 0; i <= slices2; i++) {
			sintemp = sinCache[i];
			costemp = cosCache[i];
			for (j = 0; j <= loops; j++) {
				radiusLow = outerRadius - deltaRadius * ((float)j / loops);
				texLow = radiusLow / outerRadius / 2;
				
				vertices[indexV] = radiusLow * sintemp;
				normals[indexV++] = 0.0f;
				vertices[indexV] = radiusLow * costemp;
				normals[indexV++] =0.0f;
				vertices[indexV] = 0.0;
				normals[indexV++] = 1.0f;
				texCoords[indexT++] = texLow * sinCache[i] + 0.5;
				texCoords[indexT++] = texLow * cosCache[i] + 0.5;

			}
		}

	int k = 0;
	for (j = 0; j < slices2; j++) {
		int row1 = j*(loops + 1);
		int row2 = (j + 1)*(loops + 1);
		for (i = 0; i < loops; i++) {
			indices[k++] = row1 + i;
			indices[k++] = row2 + i + 1;
			indices[k++] = row2 + i;
			indices[k++] = row1 + i;
			indices[k++] = row1 + i + 1;
			indices[k++] = row2 + i + 1;
		}
	}
}

void drawDisk(double innerRadius, double outerRadius, int slices, int loops, float *vertices, float *normals, float *texCoords, unsigned short *indices)
{
	drawPartialDisk(innerRadius, outerRadius, slices, loops, 0.0, 360.0, vertices, normals, texCoords, indices);
}

void drawTrapezoidalCube(double baseLength, double baseWidth, double baseHeight, float *vertices, float *normals, float *texCoords, unsigned short *indices)
{	float cubeVertices[] = {
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,

		//bottom face
		0.75f, -1.0f, -0.75f,
		-0.75f, -1.0f, -0.75f,
		-0.75f, -1.0f, 0.75f,
		0.75f, -1.0f, 0.75f,

		//front face
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-0.75f, -1.0f, 0.75f,
		0.75f, -1.0f, 0.75f,

		//back face
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-0.75f, -1.0f, -0.75f,
		0.75f, -1.0f, -0.75f,

		//right face
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		0.75f, -1.0f, 0.75f,
		0.75f, -1.0f, -0.75f,

		//left face
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-0.75f, -1.0f, 0.75f,
		-0.75f, -1.0f, -0.75f
	};

	float cubeNormal[] = {
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f
	};

	float cubeTexCoords[] = {
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

		0.0f, 0.0f, 
		1.0f, 0.0f, 
		1.0f, 1.0f, 
		0.0f, 1.0f, 

		1.0f, 0.0f, 
		1.0f, 1.0f, 
		0.0f, 1.0f, 
		0.0f, 0.0f, 

		1.0f, 0.0f, 
		1.0f, 1.0f, 
		0.0f, 1.0f, 
		0.0f, 0.0f, 

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
	};

	unsigned short cubeIndices[] = {
		 0, 2, 3,
		 0, 1, 3,
		 4, 6, 7,
		 4, 5, 7,
		 8, 9, 10,
		 11, 8, 10,
		 12, 13, 14,
		 15, 12, 14,
		 16, 17, 18,
		 16, 19, 18,
		 20, 21, 22,
		 20, 23, 22 
	};
	for(int i=0;i<sizeof(cubeVertices)/ sizeof(cubeVertices[0]);i++)
		vertices[i] = cubeVertices[i];
	for (int i = 0; i<sizeof(cubeNormal)/ sizeof(cubeNormal[0]); i++)
		normals[i] = cubeNormal[i];
	for (int i = 0; i<sizeof(cubeTexCoords)/ sizeof(cubeTexCoords[0]); i++)
		texCoords[i] = cubeTexCoords[i];
	for (int i = 0; i<sizeof(cubeIndices)/sizeof(cubeIndices[0]); i++)
		indices = cubeIndices;
}


void drawCube(double baseLength, double baseWidth, double baseHeight, float *vertices, float *normals, float *texCoords, unsigned short *indices)
{
	float cubeVertices[] = {
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,

		//bottom face
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,

		//front face
		1.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,

		//back face
		1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		//right face
		1.0f, 1.0f, -1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 1.0f,
		1.0f, -1.0f, -1.0f,

		//left face
		-1.0f, 1.0f, -1.0f,
		-1.0f, 1.0f, 1.0f,
		-1.0f, -1.0f, 1.0f,
		-1.0f, -1.0f, -1.0f
	};

	float cubeNormal[] = {
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		0.0f, -1.0f, 0.0f,

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,

		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,
		0.0f, 0.0f, -1.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f,
		-1.0f, 0.0f, 0.0f
	};

	float cubeTexCoords[] = {
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,

		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 0.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,

		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
		0.0f, 0.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,
	};

	unsigned short cubeIndices[] = {
		0, 2, 3,
		0, 1, 3,
		4, 6, 7,
		4, 5, 7,
		8, 9, 10,
		11, 8, 10,
		12, 13, 14,
		15, 12, 14,
		16, 17, 18,
		16, 19, 18,
		20, 21, 22,
		20, 23, 22
	};
	for (int i = 0; i<sizeof(cubeVertices) / sizeof(cubeVertices[0]); i++)
		vertices[i] = cubeVertices[i];
	for (int i = 0; i<sizeof(cubeNormal) / sizeof(cubeNormal[0]); i++)
		normals[i] = cubeNormal[i];
	for (int i = 0; i<sizeof(cubeTexCoords) / sizeof(cubeTexCoords[0]); i++)
		texCoords[i] = cubeTexCoords[i];
	for (int i = 0; i<sizeof(cubeIndices) / sizeof(cubeIndices[0]); i++)
		indices = cubeIndices;
}
