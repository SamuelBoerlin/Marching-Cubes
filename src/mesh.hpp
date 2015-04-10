//Author: Benny Bobaganoosh

#ifndef MESH_H
#define MESH_H

#include <glm\glm.hpp>
#include <GL\glew.h>

class Mesh {
	public:
		/***Default constructors/destructors***/
		/*Default costructor*/
		Mesh(GLfloat* vertices, GLfloat* colors, GLfloat* normals, unsigned int numVertices, unsigned int numNormals);
		/*Default destructor*/
		virtual ~Mesh();

		/***Prototypes***/
		/*Draws the mesh*/
		void Draw();

		/***Default constructors/destructors***/
		/*Mesh copy constructor*/
		Mesh(const Mesh& other) {}
		/*Mesh copy constructor*/
		void operator=(const Mesh& other) {}
	protected:
	private:
		/***Variables***/
		enum {
			POSITION_VB,
			COLOR_VB,
			NORMAL_VB,
			TEXTURE_VB,
			/*Always last enum, will always have the number of enums in this block*/
			NUM_BUFFERS
		};
		/*Stores the vertex array object*/
		GLuint m_vertexArrayObject;
		/*Stores the vertex array buffers*/
		GLuint m_vertexArrayBuffers[NUM_BUFFERS];
		/*Stores how many objects in the vertex array should be drawn*/
		unsigned int m_drawCount;

		/***Prototypes***/
};

#endif