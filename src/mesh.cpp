//Author: Benny Bobaganoosh

#include "mesh.hpp"
#include <map>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <stdlib.h>

Mesh::Mesh(GLfloat* vertices, GLfloat* colors, GLfloat* normals, unsigned int numVertices, unsigned int numNormals) {
	m_drawCount = /*numIndices*/numVertices;

	//Creates a vertex array object (VBO)
	glGenVertexArrays(1, &m_vertexArrayObject);
	glBindVertexArray(m_vertexArrayObject);

	//Generates new vertex buffer(s)
	glGenBuffers(NUM_BUFFERS, m_vertexArrayBuffers);

	//Creates the position vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[POSITION_VB]);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Creates the color vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[COLOR_VB]);
	glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(GLfloat), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Creates the normal vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[NORMAL_VB]);
	glBufferData(GL_ARRAY_BUFFER, numNormals * sizeof(GLfloat), normals, GL_STATIC_DRAW);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

	//Creates the texture vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexArrayBuffers[TEXTURE_VB]);
	GLfloat* texCoords = new GLfloat[numVertices * 2];
	for(int i = 0; i < numVertices * 2; i++) {
		texCoords[i] = 0.0f;
	}
	glBufferData(GL_ARRAY_BUFFER, numVertices * 2 * sizeof(GLfloat), texCoords, GL_STATIC_DRAW);
	delete[] texCoords;
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, 0);

	//Enables vertex arrays
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	//Binds vertex arrays
	glBindVertexArray(0);
}

Mesh::~Mesh()
{
	glDeleteBuffers(NUM_BUFFERS, m_vertexArrayBuffers);
	glDeleteVertexArrays(1, &m_vertexArrayObject);
}

void Mesh::Draw()
{
	glBindVertexArray(m_vertexArrayObject);

	//glDrawArrays(GL_QUADS, 0, m_drawCount);
	glDrawArrays(GL_TRIANGLES, 0, m_drawCount);
	//glDrawElementsBaseVertex(GL_TRIANGLES, m_drawCount, GL_UNSIGNED_INT, 0, 0);

	glBindVertexArray(0);
}
