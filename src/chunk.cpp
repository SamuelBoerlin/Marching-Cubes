//Author: Samuel Börlin

#include "chunk.hpp"
#include "simplemc.hpp"
#include "engine.hpp"
#include "world.hpp"

#include "perlinnoise2.hpp"
#include "perlinnoise.hpp"

PerlinNoise2 pn3;

Chunk::Chunk(int x, int y, int z, World* pWorld) {
	pn3.init(5);

	//m_pVoxels = new Voxel[CHUNKSIZEX * CHUNKSIZEY * CHUNKSIZEZ];
	m_pVoxels = new Voxel*[CHUNKSIZEX * CHUNKSIZEY * CHUNKSIZEZ];
	for(int i = 0; i < CHUNKSIZEX * CHUNKSIZEY * CHUNKSIZEZ; i++) {
		m_pVoxels[i] = new Voxel(0.0f, 0, glm::vec3(0, 0, 0));
	}
	m_chunkPos.x = x;
	m_chunkPos.y = y;
	m_chunkPos.z = z;
	m_pChunkMesh = nullptr;
	m_pWorld = pWorld;
	m_voxelsGenerated = false;
	m_meshBuilt = false;
}

Chunk::~Chunk() {
	for(int i = 0; i < CHUNKSIZEX * CHUNKSIZEY * CHUNKSIZEZ; i++) {
		Voxel* pVoxel = m_pVoxels[i];
		delete pVoxel;
		pVoxel = nullptr;
	}
	delete[] m_pVoxels;
	if(m_pChunkMesh != nullptr) delete m_pChunkMesh;
}

float DensityAt2(int x, int y, int z) {
	if(y > 0) return (pn3.FractalNoise3D(x/32.0f, y/32.0f, z/32.0f, 5, 4, 15))/(y/8.0f);
	return 8.0f;
}

float OctaveFractalNoise3D(float x, float y, float z, int octaves) {
	if(y <= 0) return 8.0f;
	float density = 0.0f;
	density += (pn3.FractalNoise3D(x/32.0f, y/32.0f, z/32.0f, octaves, 4.0f, 20.0f))/(y/8.0f);
	return density;
}

double MandelBulbFractalDensity(double x, double y, double z, int iterations, double order) {
	double r = sqrt(x*x + y*y + z*z );
	double theta = atan2(sqrt(x*x + y*y), z);
	double phi = atan2(y,x);

	for(int i = 0; i < iterations; i++) {
		x = pow(r, order) * sin(theta*order) * cos(phi*order);
		y = pow(r, order) * sin(theta*order) * sin(phi*order);
		z = pow(r, order) * cos(theta*order);

		r = sqrt(x*x + y*y + z*z );
		theta = atan2(sqrt(x*x + y*y), z);
		phi = atan2(y,x);
	}

	return theta;
}

float PerlinHeight(float x, float z, float max, double scale) {
	return max/2.0f + PerlinNoise::GetHeight(x, z, max/2.0f, scale);
}
float PerlinDensity(float x, float y, float z, float max, double scale) {
	double exactHeight = PerlinHeight(x, z, max, scale);
	int height = (int)ceil(exactHeight);
	return (1.0f - (height - exactHeight)) + Engine::ISOLEVEL;
}

void Chunk::GenerateVoxels() {
	m_voxelsGenerated = false;
	/*for(int x = 0; x < CHUNKSIZEX; ++x) {
		for(int z = 0; z < CHUNKSIZEZ; ++z) {
			int px = x + m_chunkPos.x*CHUNKSIZEX + 3000000;
			int pz = z + m_chunkPos.z*CHUNKSIZEZ + 3000000;
			double exactHeight = PerlinHeight(px, pz, CHUNKSIZEY, 0.05);
			int height = ((int)ceil(exactHeight));
			for(int y = 0; y < height; ++y) {
				float density = (1.0f - (height - exactHeight)) + Engine::ISOLEVEL;
				//if(DensityAt2(x*2.0f, y*2.0f, z*2.0f)*2.0f < Engine::ISOLEVEL) {
					//density += DensityAt2(x + m_chunkPos.x*CHUNKSIZEX + 3000000, y + m_chunkPos.y*CHUNKSIZEZ + 3000000, z + m_chunkPos.z*CHUNKSIZEZ + 3000000)/4.0f;
				//}
				//m_data[x][y][z] = density;
				//std::cout << pn2.Noise3D(x, y, z) << std::endl;
				//m_data[x][y][z] = DensityAt(x, y, z);
				glm::vec3 voxelNormal(PerlinDensity(px-1, y, pz, CHUNKSIZEY, 0.05)-PerlinDensity(px+1, y, pz, CHUNKSIZEY, 0.05),PerlinDensity(px, y-1, pz, CHUNKSIZEY, 0.05)-PerlinDensity(px, y+1, pz, CHUNKSIZEY, 0.05),PerlinDensity(px, y, pz-1, CHUNKSIZEY, 0.05)-PerlinDensity(px, y, pz+1, CHUNKSIZEY, 0.05));
				Voxel* voxel = GetVoxel(x, y, z);
				voxel->density = density;
				voxel->normal = voxelNormal;
			}
		}
	}*/
	/*for(int x = 0; x < CHUNKSIZEX; ++x) {
		for(int z = 0; z < CHUNKSIZEZ; ++z) {
			for(int y = 0; y < CHUNKSIZEY; ++y) {
				float density = 0.0f;
				float px = x + m_chunkPos.x*CHUNKSIZEX + 3000000;
				float py = y + m_chunkPos.y*CHUNKSIZEY;
				float pz = z + m_chunkPos.z*CHUNKSIZEZ + 3000000;
				density += DensityAt2(px, py, pz)/8.0f;
				//density += MandelBulbFractalDensity(px/16.0f, py/16.0f, pz/16.0f, 8, 1);

				glm::vec3 voxelNormal(DensityAt2(px-1, py, pz)/8.0f-DensityAt2(px+1, py, pz)/8.0f,DensityAt2(px, py-1, pz)/8.0f-DensityAt2(px, py+1, pz)/8.0f,DensityAt2(px, py, pz-1)/8.0f-DensityAt2(px, py, pz+1)/8.0f);
				voxelNormal = glm::normalize(voxelNormal);

				Voxel* voxel = GetVoxel(x, y, z);
				voxel->density = density;
				voxel->normal = voxelNormal;
			}
		}
	}*/
	/*for(int x = 0; x < CHUNKSIZEX; ++x) {
		for(int z = 0; z < CHUNKSIZEZ; ++z) {
			for(int y = 0; y < CHUNKSIZEY; ++y) {
				float density = 0.0f;
				float px = x + m_chunkPos.x*CHUNKSIZEX + 3000000;
				float py = y + m_chunkPos.y*CHUNKSIZEY;
				float pz = z + m_chunkPos.z*CHUNKSIZEZ + 3000000;
				density += OctaveFractalNoise3D(px, py, pz, 8)/8.0f;

				glm::vec3 voxelNormal(OctaveFractalNoise3D(px-1, py, pz, 8)/8.0f-OctaveFractalNoise3D(px+1, py, pz, 8)/8.0f,OctaveFractalNoise3D(px, py-1, pz, 8)/8.0f-OctaveFractalNoise3D(px, py+1, pz, 8)/8.0f,OctaveFractalNoise3D(px, py, pz-1, 8)/8.0f-OctaveFractalNoise3D(px, py, pz+1, 8)/8.0f);
				voxelNormal = glm::normalize(voxelNormal);

				Voxel* voxel = GetVoxel(x, y, z);
				voxel->density = density;
				voxel->normal = voxelNormal;
				//voxel->normal = glm::vec3(0, 0, 0);
			}
		}
	}*/
	for(int x = 0; x < CHUNKSIZEX; ++x) {
		for(int z = 0; z < CHUNKSIZEZ; ++z) {
			for(int y = 0; y < CHUNKSIZEY; ++y) {
				float px = x + m_chunkPos.x*CHUNKSIZEX;
				float py = y + m_chunkPos.y*CHUNKSIZEY;
				float pz = z + m_chunkPos.z*CHUNKSIZEZ;
				if(py < 1) continue;
				//glm::vec3 voxelNormal(OctaveFractalNoise3D(px-1, py, pz, 8)/8.0f-OctaveFractalNoise3D(px+1, py, pz, 8)/8.0f,OctaveFractalNoise3D(px, py-1, pz, 8)/8.0f-OctaveFractalNoise3D(px, py+1, pz, 8)/8.0f,OctaveFractalNoise3D(px, py, pz-1, 8)/8.0f-OctaveFractalNoise3D(px, py, pz+1, 8)/8.0f);
				//voxelNormal = glm::normalize(voxelNormal);

				Voxel* voxel = GetVoxel(x, y, z);
				voxel->density = (sin(sqrt(px*px + pz*pz)/5.0f)+1.0f)/(py/2.0f);
				glm::vec3 voxelNormal((sin(sqrt((px-1)*(px-1) + pz*pz)/5.0f)+1.0f)/(py/2.0f)-(sin(sqrt((px+1)*(px+1) + pz*pz)/5.0f)+1.0f)/(py/2.0f),
										(sin(sqrt(px*px + pz*pz)/5.0f)+1.0f)/((py-1)/2.0f)-(sin(sqrt(px*px + pz*pz)/5.0f)+1.0f)/((py+1)/2.0f), 
										(sin(sqrt(px*px + (pz+1)*(pz+1))/5.0f)+1.0f)/(py/2.0f)-(sin(sqrt(px*px + (pz+1)*(pz+1))/5.0f)+1.0f)/(py/2.0f));
				voxel->normal = glm::normalize(voxelNormal);
				//std::cout << voxel->normal.x << " " << voxel->normal.y << " " << voxel->normal.z << std::endl;
			}
		}
	}
	//RebuildNeighbours();
	m_voxelsGenerated = true;
}

void Chunk::RebuildNeighbours() {
	Chunk* pChunkX1 = m_pWorld->GetChunk(m_chunkPos.x+1, m_chunkPos.y, m_chunkPos.z);
	Chunk* pChunkX2 = m_pWorld->GetChunk(m_chunkPos.x-1, m_chunkPos.y, m_chunkPos.z);
	Chunk* pChunkY1 = m_pWorld->GetChunk(m_chunkPos.x, m_chunkPos.y+1, m_chunkPos.z);
	Chunk* pChunkY2 = m_pWorld->GetChunk(m_chunkPos.x, m_chunkPos.y-1, m_chunkPos.z);
	Chunk* pChunkZ1 = m_pWorld->GetChunk(m_chunkPos.x, m_chunkPos.y, m_chunkPos.z+1);
	Chunk* pChunkZ2 = m_pWorld->GetChunk(m_chunkPos.x, m_chunkPos.y, m_chunkPos.z-1);
	if(pChunkX1 != nullptr) {
		pChunkX1->BuildMesh();
	}
	if(pChunkX2 != nullptr) {
		pChunkX2->BuildMesh();
	}
	if(pChunkY1 != nullptr) {
		pChunkY1->BuildMesh();
	}
	if(pChunkY2 != nullptr) {
		pChunkY2->BuildMesh();
	}
	if(pChunkZ1 != nullptr) {
		pChunkZ1->BuildMesh();
	}
	if(pChunkZ2 != nullptr) {
		pChunkZ2->BuildMesh();
	}
}

void Chunk::BuildMesh() {
	m_meshBuilt = false;
	if(m_pChunkMesh != nullptr) {
		delete m_pChunkMesh;
	}
	SimpleMC simpleMarchingCubes;
	std::vector<GLfloat> verts;
	std::vector<GLfloat> normals;
	std::vector<GLfloat> colors;
	SimpleMC::TRIANGLE cTri;
	glm::vec3 cVert;
	glm::vec3 cNorm;
	SimpleMC::GRIDCELL cell;
	for(int y = -1; y < CHUNKSIZEY; y++) {
		for(int x = -1; x < CHUNKSIZEX; x++) {
			for(int z = -1; z < CHUNKSIZEZ; z++) {
				VoxelPosition vp1(x, y, z);
				Voxel* v1 = GetVoxelFromPosition(vp1);
				VoxelPosition vp2(x+1, y, z);
				Voxel* v2 = GetVoxelFromPosition(vp2);
				VoxelPosition vp3(x+1, y+1, z);
				Voxel* v3 = GetVoxelFromPosition(vp3);
				VoxelPosition vp4(x, y+1, z);
				Voxel* v4 = GetVoxelFromPosition(vp4);
				VoxelPosition vp5(x, y, z+1);
				Voxel* v5 = GetVoxelFromPosition(vp5);
				VoxelPosition vp6(x+1, y, z+1);
				Voxel* v6 = GetVoxelFromPosition(vp6);
				VoxelPosition vp7(x+1, y+1, z+1);
				Voxel* v7 = GetVoxelFromPosition(vp7);
				VoxelPosition vp8(x, y+1, z+1);
				Voxel* v8 = GetVoxelFromPosition(vp8);

				cell.densities[0].x = vp1.x;
				cell.densities[0].y = vp1.y;
				cell.densities[0].z = vp1.z;
				if(v1 != nullptr) {
					cell.densities[0].w = v1->density;
					cell.normals[0] = v1->normal;
				} else {
					cell.densities[0].w = 0.0f;
					cell.normals[0] = glm::vec3(0, 0, 0);
				}

				cell.densities[1].x = vp2.x;
				cell.densities[1].y = vp2.y;
				cell.densities[1].z = vp2.z;
				if(v2 != nullptr) {
					cell.densities[1].w = v2->density;
					cell.normals[1] = v2->normal;
				} else {
					cell.densities[1].w = 0.0f;
					cell.normals[1] = glm::vec3(0, 0, 0);
				}

				cell.densities[2].x = vp3.x;
				cell.densities[2].y = vp3.y;
				cell.densities[2].z = vp3.z;
				if(v3 != nullptr) {
					cell.densities[2].w = v3->density;
					cell.normals[2] = v3->normal;
				} else {
					cell.densities[2].w = 0.0f;
					cell.normals[2] = glm::vec3(0, 0, 0);
				}

				cell.densities[3].x = vp4.x;
				cell.densities[3].y = vp4.y;
				cell.densities[3].z = vp4.z;
				if(v4 != nullptr) {
					cell.densities[3].w = v4->density;
					cell.normals[3] = v4->normal;
				} else {
					cell.densities[3].w = 0.0f;
					cell.normals[3] = glm::vec3(0, 0, 0);
				}

				cell.densities[4].x = vp5.x;
				cell.densities[4].y = vp5.y;
				cell.densities[4].z = vp5.z;
				if(v5 != nullptr) {
					cell.densities[4].w = v5->density;
					cell.normals[4] = v5->normal;
				} else {
					cell.densities[4].w = 0.0f;
					cell.normals[4] = glm::vec3(0, 0, 0);
				}

					
				cell.densities[5].x = vp6.x;
				cell.densities[5].y = vp6.y;
				cell.densities[5].z = vp6.z;
				if(v6 != nullptr) {
					cell.densities[5].w = v6->density;
					cell.normals[5] = v6->normal;
				} else {
					cell.densities[5].w = 0.0f;
					cell.normals[5] = glm::vec3(0, 0, 0);
				}

				cell.densities[6].x = vp7.x;
				cell.densities[6].y = vp7.y;
				cell.densities[6].z = vp7.z;
				if(v7 != nullptr) {
					cell.densities[6].w = v7->density;
					cell.normals[6] = v7->normal;
				} else {
					cell.densities[6].w = 0.0f;
					cell.normals[6] = glm::vec3(0, 0, 0);
				}

				cell.densities[7].x = vp8.x;
				cell.densities[7].y = vp8.y;
				cell.densities[7].z = vp8.z;
				if(v8 != nullptr) {
					cell.densities[7].w = v8->density;
					cell.normals[7] = v8->normal;
				} else {
					cell.densities[7].w = 0.0f;
					cell.normals[7] = glm::vec3(0, 0, 0);
				}

				/*int cellIndex = 0;
				for(int cx = 0; cx < 1; cx++) {
					for(int cy = 0; cy < 1; cy++) {
						for(int cz = 0; cz < 1; cz++) {
							VoxelPosition voxelPos(x+cx, y+cy, z+cz);
							Voxel* voxel = GetVoxelFromPosition(voxelPos);
							cell.densities[cellIndex].x = voxelPos.x;
							cell.densities[cellIndex].y = voxelPos.y;
							cell.densities[cellIndex].z = voxelPos.z;
							if(voxel != nullptr) {
								cell.densities[cellIndex].w = voxel->density;
								cell.normals[cellIndex] = voxel->normal;
							} else {
								cell.densities[cellIndex].w = 0.0f;
								cell.normals[cellIndex] = glm::vec3(0, 0, 0);
							}
							cellIndex++;
						}
					}
				}*/

				int count = 0;
				SimpleMC::TRIANGLE* triangleArray = simpleMarchingCubes.PolygoniseCell(cell, Engine::ISOLEVEL, count);

				for(int i = 0; i < count; i++) {
					cTri = triangleArray[i];
					for(int vec = 0; vec < 3; vec++) {
						cVert = cTri.vertices[vec];
						cNorm = cTri.normals[vec];
						verts.push_back(cVert.x + m_chunkPos.x*CHUNKSIZEX);
						verts.push_back(cVert.y + m_chunkPos.y*CHUNKSIZEY);
						verts.push_back(cVert.z + m_chunkPos.z*CHUNKSIZEZ);
						normals.push_back(cNorm.x);
						normals.push_back(cNorm.y);
						normals.push_back(cNorm.z);
						colors.push_back(1.0f - (cNorm.y+1.0f)/2.0f);
						colors.push_back((cNorm.y+1.0f)/2.0f);
						colors.push_back(0);
					}
				}

				delete[] triangleArray;
			}
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

	m_pChunkMesh = pMesh;

	m_meshBuilt = true;
}

void Chunk::Render() {
	if(m_pChunkMesh != nullptr) {
		m_pChunkMesh->Draw();
	}
}

inline Voxel* Chunk::GetVoxelFromPosition(VoxelPosition vp) {
	if(vp.x >= 0 && vp.x < CHUNKSIZEX && vp.y >= 0 && vp.y < CHUNKSIZEY && vp.z >= 0 && vp.z < CHUNKSIZEZ) {
		return GetVoxel(vp.x, vp.y, vp.z);
	} else {
		return m_pWorld->GetVoxel(vp.x + m_chunkPos.x * CHUNKSIZEX, vp.y + m_chunkPos.y * CHUNKSIZEY, vp.z + m_chunkPos.z * CHUNKSIZEZ);
	}
	return nullptr;
}