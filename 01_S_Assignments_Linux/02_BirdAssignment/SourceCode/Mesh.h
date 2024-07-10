#ifndef _MESH_H
#define _MESH_H

class Mesh
{
	GLuint vao;
	GLuint vbo_Position;
	GLuint vbo_Normal;
	GLuint vbo_TexCoord;
	GLuint vbo_Element;

	GLuint vPositionSize;
	GLuint vNormalSize;
	GLuint vTexCoordSize;
	GLuint vElementSize;

public:
	void RenderMesh(int size) {
		glBindVertexArray(vao);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element);
		glDrawElements(GL_TRIANGLES, vElementSize, GL_UNSIGNED_SHORT, 0);
		glBindVertexArray(0);
	}

	void RenderMeshByArray(int Size)
	{
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 4, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 8, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 12, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 16, 4);
		glDrawArrays(GL_TRIANGLE_FAN, 20, 4);
		glBindVertexArray(0);
	}
	void CreateMesh(float *vertices, float *normals, float *texCoords, unsigned short *indices, int verticesSize, int normalsSize, int texCoordsSize, int indicesSize) {
		vPositionSize=verticesSize;
		vNormalSize=normalsSize;
		vTexCoordSize=texCoordsSize;
		vElementSize=indicesSize;

		
		//Rectangle vao
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		//Position
		glGenBuffers(1, &vbo_Position);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_Position);
		glBufferData(GL_ARRAY_BUFFER, verticesSize*sizeof(float), vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//Normals
		glGenBuffers(1, &vbo_Normal);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_Normal);
		glBufferData(GL_ARRAY_BUFFER, normalsSize * sizeof(float), normals, GL_STATIC_DRAW);
		glVertexAttribPointer(AMC_ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(AMC_ATTRIBUTE_NORMAL);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//TextCoord
		glGenBuffers(1, &vbo_TexCoord);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_TexCoord);
		glBufferData(GL_ARRAY_BUFFER, texCoordsSize * sizeof(float), texCoords, GL_STATIC_DRAW);
		glVertexAttribPointer(AMC_ATTRIBUTE_TEXCOORD0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(AMC_ATTRIBUTE_TEXCOORD0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		//Elements
		glGenBuffers(1, &vbo_Element);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_Element);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesSize * sizeof(unsigned short), indices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void ClearMesh() {
		if (vbo_Position)
		{
			glDeleteBuffers(1, &vbo_Position);
			vbo_Position = 0;
		}

		if (vbo_Normal)
		{
			glDeleteBuffers(1, &vbo_Normal);
			vbo_Normal = 0;
		}
		if (vbo_TexCoord)
		{
			glDeleteBuffers(1, &vbo_TexCoord);
			vbo_TexCoord = 0;
		}
		if (vbo_Element)
		{
			glDeleteBuffers(1, &vbo_Element);
			vbo_Element = 0;
		}

		if (vao)
		{
			glDeleteBuffers(1, &vao);
			vao = 0;
		}
	}
};

#endif 
