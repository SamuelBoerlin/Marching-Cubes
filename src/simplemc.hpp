//Author: Samuel Börlin, Paul Bourke http://paulbourke.net/geometry/polygonise/

#ifndef SIMPLEMC_H
#define SIMPLEMC_H

#include <glm\vec4.hpp>
#include <glm\vec3.hpp>
#include <math.h>
#include <glm\glm.hpp>
#include <glm\detail\_swizzle.hpp>
#include <glm\detail\_swizzle_func.hpp>
#include <vector>
#include "mesh.hpp"
#include "mctables.hpp"
#include <iostream>
#include "world.hpp"

#define ABS(x) (x < 0 ? -(x) : (x))

class SimpleMC {
public:
	struct DENSITYPOINT {
		float density;
		glm::vec3 normal;
		DENSITYPOINT(float d, glm::vec3 n) {
			density = d;
			normal = n;
		}
	};

	typedef struct {
		glm::vec3 vertices[3];
		glm::vec3 normals[3];
	} TRIANGLE;

	typedef struct {
		glm::vec4 densities[8];
		glm::vec3 normals[8];
	} GRIDCELL;

	void SetDensitySphere(int cx, int cy, int cz, float radius, float min, float max, World* pWorld)  {
		float densityDiff = max - min;
		for(int i = cx - radius - 1; i < cx + radius + 1; i++) {
			for(int j = cy - radius - 1; j < cy + radius + 1; j++) {
				for(int k = cz - radius - 1; k < cz + radius + 1; k++) {
					float dist = sqrt((i-cx)*(i-cx)+(j-cy)*(j-cy)+(k-cz)*(k-cz));
					if(dist <= (int)ceil(radius)) {
						Voxel* pVoxel = pWorld->GetVoxel(i, j, k);
						if(pVoxel == nullptr) continue;
						float density = max - ((float)ceil(radius)-dist)/(float)ceil(radius)*densityDiff;
						if(density < min) density = min;
						pVoxel->density = density;
						//pVoxel->normal = glm::normalize(glm::vec3(cx - i, cy- j, cz - k));
						pVoxel->normal = glm::vec3(0, 0, 0);
					}
				}
			}
		}
	}

	void IncrDensitySphere(int cx, int cy, int cz, float radius, float incr, float min, float max, World* pWorld)  {
		cy+=1;
		cx+=1;
		cz+=1;
		for(int i = cx - radius - 1; i < cx + radius + 1; i++) {
			for(int j = cy - radius - 1; j < cy + radius + 1; j++) {
				for(int k = cz - radius - 1; k < cz + radius + 1; k++) {
					float dist = sqrt((i-cx)*(i-cx)+(j-cy)*(j-cy)+(k-cz)*(k-cz));
					if(dist <= (int)ceil(radius)) {
						Voxel* pVoxel = pWorld->GetVoxel(i, j, k);
						if(pVoxel == nullptr) continue;
						float density = pVoxel->density;
						float increase = incr*((float)ceil(radius)*((float)ceil(radius)-dist));
						density += increase;
						if(density > max) {
							density = max;
						} else if(density < min) {
							density = min;
						}
						pVoxel->density = density;
						//pVoxel->normal = -glm::normalize(glm::vec3(cx - i, cy- j, cz - k));
						pVoxel->normal = glm::vec3(0, 0, 0);
					}
				}
			}
		}
	}

	void SmoothDensitySphere(int cx, int cy, int cz, float radius, float min, World* pWorld) {
		cy+=1;
		cx+=1;
		cz+=1;
		float average = 0.0f;
		int densityPoints = 0;
		for(int i = cx - radius - 1; i < cx + radius + 1; i++) {
			for(int j = cy - radius - 1; j < cy + radius + 1; j++) {
				for(int k = cz - radius - 1; k < cz + radius + 1; k++) {
					float dist = sqrt((i-cx)*(i-cx)+(j-cy)*(j-cy)+(k-cz)*(k-cz));
					if(dist <= (int)ceil(radius)) {
						Voxel* pVoxel = pWorld->GetVoxel(i, j, k);
						if(pVoxel == nullptr) continue;
						float density = pVoxel->density;
						if(density < min) continue;
						average += density;
						densityPoints++;
					}
				}
			}
		}
		average /= (float)densityPoints;
		for(int i = cx - radius - 1; i < cx + radius + 1; i++) {
			for(int j = cy - radius - 1; j < cy + radius + 1; j++) {
				for(int k = cz - radius - 1; k < cz + radius + 1; k++) {
					float dist = sqrt((i-cx)*(i-cx)+(j-cy)*(j-cy)+(k-cz)*(k-cz));
					if(dist <= (int)ceil(radius)) {
						Voxel* pVoxel = pWorld->GetVoxel(i, j, k);
						if(pVoxel == nullptr) continue;
						float density = pVoxel->density;
						if(density < min) continue;
						if(density > average) {
							density -= 0.01f;
						} else if(density < average) {
							density += 0.01f;
						}
						pVoxel->density = density;
						//pVoxel->normal = glm::normalize(glm::vec3(cx - i, cy- j, cz - k));
						pVoxel->normal = glm::vec3(0, 0, 0);
					}
				}
			}
		}
	}

	void SetDensityCube(int x, int y, int z, int width, int height, int depth, float density, World* pWorld) {
		for(int i = x; i < x + width; i++) {
			for(int j = y; j < y + height; j++) {
				for(int k = z; k < z + depth; k++) {
					//data[i][j][k] = density;
					Voxel* pVoxel = pWorld->GetVoxel(i, j, k);
					pVoxel->density = density;
					pVoxel->normal = glm::vec3(0, 0, 0);
				}
			}
		}
	}

	Mesh* MeshField(float*** data, int sizeX, int sizeY, int sizeZ, int gridSize, float isoLevel) {
		std::vector<TRIANGLE> triangles = PolygoniseField(data, sizeX, sizeY, sizeZ, gridSize, isoLevel);

		std::vector<GLfloat> verts;
		std::vector<GLfloat> normals;
		std::vector<GLfloat> colors;

		TRIANGLE cTri;
		glm::vec3 cVert;
		glm::vec3 cNorm;
		for(int i = 0; i < triangles.size(); i++) {
			for(int v = 0; v < 3; v++) {
				cTri = triangles[i];
				cVert = cTri.vertices[v];
				cNorm = cTri.normals[v];
				verts.push_back(cVert.x);
				verts.push_back(cVert.y);
				verts.push_back(cVert.z);
				normals.push_back(cNorm.x);
				normals.push_back(cNorm.y);
				normals.push_back(cNorm.z);
				colors.push_back(1.0f - (cNorm.y+1.0f)/2.0f);
				colors.push_back((cNorm.y+1.0f)/2.0f);
				colors.push_back(0);
			}
		}

		GLfloat* vertexArray = new GLfloat[verts.size()];
		std::copy(verts.begin(), verts.end(), vertexArray);

		GLfloat* colorArray = new GLfloat[colors.size()];
		std::copy(colors.begin(), colors.end(), colorArray);

		GLfloat* normalArray = new GLfloat[normals.size()];
		std::copy(normals.begin(), normals.end(), normalArray);

		Mesh* pMesh = new Mesh(vertexArray, colorArray, normalArray, verts.size(), normals.size());

		delete[] vertexArray;
		delete[] colorArray;
		delete[] normalArray;

		return pMesh;
	}

	std::vector<TRIANGLE> PolygoniseField(float*** scalarField, int sizeX, int sizeY, int sizeZ, int gridSize, float isoLevel) {
		GRIDCELL cell;
		std::vector<TRIANGLE> triangleVector;
		for(int y = 0; y < sizeY - gridSize; y++) {
			for(int x = 0; x < sizeX - gridSize; x++) {
				for(int z = 0; z < sizeZ - gridSize; z++) {
					cell.densities[0].x = x;
					cell.densities[0].y = y;
					cell.densities[0].z = z;
					cell.densities[0].w = scalarField[x][y][z];

					cell.densities[1].x = x + gridSize;
					cell.densities[1].y = y;
					cell.densities[1].z = z;
					cell.densities[1].w = scalarField[x + gridSize][y][z];

					cell.densities[2].x = x + gridSize;
					cell.densities[2].y = y + gridSize;
					cell.densities[2].z = z;
					cell.densities[2].w = scalarField[x + gridSize][y + gridSize][z];

					cell.densities[3].x = x;
					cell.densities[3].y = y + gridSize;
					cell.densities[3].z = z;
					cell.densities[3].w = scalarField[x][y + gridSize][z];

					cell.densities[4].x = x;
					cell.densities[4].y = y;
					cell.densities[4].z = z + gridSize;
					cell.densities[4].w = scalarField[x][y][z + gridSize];

					cell.densities[5].x = x + gridSize;
					cell.densities[5].y = y;
					cell.densities[5].z = z + gridSize;
					cell.densities[5].w = scalarField[x + gridSize][y][z + gridSize];

					cell.densities[6].x = x + gridSize;
					cell.densities[6].y = y + gridSize;
					cell.densities[6].z = z + gridSize;
					cell.densities[6].w = scalarField[x + gridSize][y + gridSize][z + gridSize];

					cell.densities[7].x = x;
					cell.densities[7].y = y + gridSize;
					cell.densities[7].z = z + gridSize;
					cell.densities[7].w = scalarField[x][y + gridSize][z + gridSize];

					int count = 0;
					TRIANGLE* triangles = PolygoniseCell(cell, isoLevel, count);

					for(int i = 0; i < count; i++) {
						triangleVector.push_back(triangles[i]);
					}

					delete[] triangles;
				}
			}
		}

		return triangleVector;
	}

	struct CellVertex {
		CellVertex(){};
		CellVertex(glm::vec3 vertex, glm::vec3 normal) {
			this->vertex = vertex;
			this->normal = normal;
		}
		glm::vec3 vertex;
		glm::vec3 normal;
	};

	TRIANGLE* PolygoniseCell(GRIDCELL cell, double isoLevel, int& nTri) {
		//Set count of triangles to 0
		nTri = 0;

		//Reserve memory for the 12 cell vertices
		CellVertex cellverts[12];

		//Reserve memory for triangles, maximum 10 are required
		TRIANGLE* triArray = new TRIANGLE[10];

		//Currently none of the vertices are inside
		//Using bit operations instead of booleans or arrays for better performance
		int cubeindex = 0x00000000;

		//Create cubeindex with bitwise OR operator (|) for every vertex
		//if (cell.densities[0].w <= isoLevel) cubeindex |= 1;   //binary: 00000001
		//if (cell.densities[1].w <= isoLevel) cubeindex |= 2;   //binary: 00000010
		//if (cell.densities[2].w <= isoLevel) cubeindex |= 4;   //binary: 00000100
		//if (cell.densities[3].w <= isoLevel) cubeindex |= 8;   //binary: 00001000
		//if (cell.densities[4].w <= isoLevel) cubeindex |= 16;  //binary: 00010000
		//if (cell.densities[5].w <= isoLevel) cubeindex |= 32;  //binary: 00100000
		//if (cell.densities[6].w <= isoLevel) cubeindex |= 64;  //binary: 01000000
		//if (cell.densities[7].w <= isoLevel) cubeindex |= 128; //binary: 10000000

		for(int i = 0; i < 8; i++) {
			if(cell.densities[i].w <= isoLevel) cubeindex |= 1<<i;
		}

		//cubeindex         00000000
		//vertex 5 inside   00010000
		//=                 00010000
		//vertex 7 inside   01000000
		//=                 01010000
		//etc.

		//Cube is entirely in/out of the surface
		if (MCTables::EDGE_TABLE[cubeindex] == 0x00000000) {
			return triArray;
		}

		//Find the vertices where the surface intersects the cube with the AND (&) operator

		//edgetable          01010000
		//check for edge 5   00010000
		//= true
		//check for edge 3   00000100
		//= false
		//etc.

		//Create intersection point with linear interpolation between the two vertices of the edge
		//Interpolated by difference between the densities and the isolevel

		if (MCTables::EDGE_TABLE[cubeindex] & 1) {
			cellverts[0] = Interpolate(isoLevel, cell.densities[0], cell.densities[1], cell.normals[0], cell.normals[1]);
		}
		if (MCTables::EDGE_TABLE[cubeindex] & 2) {
			cellverts[1] = Interpolate(isoLevel, cell.densities[1], cell.densities[2], cell.normals[1], cell.normals[2]);
		}
		if (MCTables::EDGE_TABLE[cubeindex] & 4) {
			cellverts[2] = Interpolate(isoLevel, cell.densities[2], cell.densities[3], cell.normals[2], cell.normals[3]);
		}
		if (MCTables::EDGE_TABLE[cubeindex] & 8) {
			cellverts[3] = Interpolate(isoLevel, cell.densities[3], cell.densities[0], cell.normals[3], cell.normals[0]);
		}
		if (MCTables::EDGE_TABLE[cubeindex] & 16) {
			cellverts[4] = Interpolate(isoLevel, cell.densities[4], cell.densities[5], cell.normals[4], cell.normals[5]);
		}
		if (MCTables::EDGE_TABLE[cubeindex] & 32) {
			cellverts[5] = Interpolate(isoLevel, cell.densities[5], cell.densities[6], cell.normals[5], cell.normals[6]);
		}
		if (MCTables::EDGE_TABLE[cubeindex] & 64) {
			cellverts[6] = Interpolate(isoLevel, cell.densities[6], cell.densities[7], cell.normals[6], cell.normals[7]);
		}
		if (MCTables::EDGE_TABLE[cubeindex] & 128) {
			cellverts[7] = Interpolate(isoLevel, cell.densities[7], cell.densities[4], cell.normals[7], cell.normals[4]);
		}
		if (MCTables::EDGE_TABLE[cubeindex] & 256) {
			cellverts[8] = Interpolate(isoLevel, cell.densities[0], cell.densities[4], cell.normals[0], cell.normals[4]);
		}
		if (MCTables::EDGE_TABLE[cubeindex] & 512) {
			cellverts[9] = Interpolate(isoLevel, cell.densities[1], cell.densities[5], cell.normals[1], cell.normals[5]);
		}
		if (MCTables::EDGE_TABLE[cubeindex] & 1024) {
			cellverts[10] = Interpolate(isoLevel, cell.densities[2], cell.densities[6], cell.normals[2], cell.normals[6]);
		}
		if (MCTables::EDGE_TABLE[cubeindex] & 2048) {
			cellverts[11] = Interpolate(isoLevel, cell.densities[3], cell.densities[7], cell.normals[3], cell.normals[7]);
		}

		//Create the triangles from the cubeindex
		//Iterate as long as valid entries are availabe (!=-1)
		for (int i = 0; MCTables::TRI_TABLE[cubeindex][i]!=-1; i += 3) {
			//Get vertex index from the entry in TRI_TABLE of cubeindex and i
			CellVertex cellVertexA = cellverts[MCTables::TRI_TABLE[cubeindex][i  ]];
			CellVertex cellVertexB = cellverts[MCTables::TRI_TABLE[cubeindex][i+1]];
			CellVertex cellVertexC = cellverts[MCTables::TRI_TABLE[cubeindex][i+2]];
			glm::vec3 vertA = cellVertexA.vertex;
			glm::vec3 vertB = cellVertexB.vertex;
			glm::vec3 vertC = cellVertexC.vertex;
			triArray[nTri].vertices[0] = vertA;
			triArray[nTri].vertices[1] = vertB;
			triArray[nTri].vertices[2] = vertC;
			glm::vec3 faceNormal;
			//Create normal by crossing the triangle vertices if the normal length of a vertex is 0
			if((cellVertexA.normal.x == 0 && cellVertexA.normal.y == 0 && cellVertexA.normal.z == 0) || 
				(cellVertexB.normal.x == 0 && cellVertexB.normal.y == 0 && cellVertexB.normal.z == 0) || 
				(cellVertexC.normal.x == 0 && cellVertexC.normal.y == 0 && cellVertexC.normal.z == 0)) {
				faceNormal = glm::normalize(glm::cross((vertA - vertC), (vertB - vertC)));
			}
			//Set vertex normal depending on wether a custom normal was set
			if((cellVertexA.normal.x == 0 && cellVertexA.normal.y == 0 && cellVertexA.normal.z == 0)) {
				triArray[nTri].normals[0] = faceNormal;
			} else {
				triArray[nTri].normals[0] = cellVertexA.normal;
			}
			if((cellVertexB.normal.x == 0 && cellVertexB.normal.y == 0 && cellVertexB.normal.z == 0)) {
				triArray[nTri].normals[1] = faceNormal;
			} else {
				triArray[nTri].normals[1] = cellVertexB.normal;
			}
			if((cellVertexC.normal.x == 0 && cellVertexC.normal.y == 0 && cellVertexC.normal.z == 0)) {
				triArray[nTri].normals[2] = faceNormal;
			} else {
				triArray[nTri].normals[2] = cellVertexC.normal;
			}
			//Add 1 to triangle count
			nTri++;
		}

		return triArray;
	}

	/*glm::vec3 Interpolate(float isolevel, glm::vec4 p1, glm::vec4 p2) {
		glm::vec3 vert1(p1);
		glm::vec3 vert2(p2);
		glm::vec3 interpolatedVec(vert1 + (vert2 - vert1) / (p2.w - p1.w) * (isolevel - p1.w));
		return interpolatedVec;
	}*/

	CellVertex Interpolate(float isolevel, glm::vec4 p1, glm::vec4 p2, glm::vec3 n1, glm::vec3 n2) {
		glm::vec3 vert1(p1);
		glm::vec3 vert2(p2);
		glm::vec3 interpolatedVec(vert1 + (vert2 - vert1) / (p2.w - p1.w) * (isolevel - p1.w));
		glm::vec3 interpolatedNormal(n1 + (n2 - n1) / (p2.w - p1.w) * (isolevel - p1.w));
		return CellVertex(interpolatedVec, interpolatedNormal);
	}
};

#endif