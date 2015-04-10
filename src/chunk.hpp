//Author: Samuel Börlin

#ifndef CHUNK_H
#define CHUNK_H

#include <glm\vec3.hpp>
#include "mesh.hpp"
#include "voxel.hpp"

class World;

class Chunk {
public:
	struct VoxelPosition {
		int x, y, z;
		VoxelPosition(){};
		VoxelPosition(int x, int y, int z) {
			this->x = x;
			this->y = y;
			this->z = z;
		}
	};
	struct ChunkPosition {
		int x, y, z;
		ChunkPosition(){};
		ChunkPosition(int x, int y, int z) {
			this->x = x;
			this->y = y;
			this->z = z;
		}
	};
	const static int CHUNKSIZEX = 16;
	const static int CHUNKSIZEY = 16;
	const static int CHUNKSIZEZ = 16;
	Chunk(int x, int y, int z, World* pWorld);
	~Chunk();
	void GenerateVoxels();
	void RebuildNeighbours();
	void BuildMesh();
	void Render();
	/**Returns the array index of the coordinates. Coordinates in world coordinates relative to the chunk**/
	inline int ToIndex(int x, int y, int z) {
		return x + CHUNKSIZEX * (y + CHUNKSIZEY * z);
	}
	inline VoxelPosition FromIndex(int index) {
		VoxelPosition pos;
		pos.z = floor(index / (CHUNKSIZEX * CHUNKSIZEY));
		pos.y = floor((index - pos.z * CHUNKSIZEX * CHUNKSIZEY) / CHUNKSIZEX);
		pos.x = index - (pos.y + CHUNKSIZEY * pos.z) * CHUNKSIZEX;
		return pos;
	}
	/**Returns the voxel. Coordinates in world coordinates relative to the chunk**/
	inline Voxel* GetVoxel(int x, int y, int z) {
		return m_pVoxels[ToIndex(x, y, z)];
	}
	inline Voxel* GetVoxel(int index) {
		return m_pVoxels[index];
	}
	/**Sets the voxel. Coordinates in world coordinates relative to the chunk**/
	inline void SetVoxel(Voxel* voxel, int x, int y, int z) {
		m_pVoxels[ToIndex(x, y, z)] = voxel;
	}
	inline void SetVoxel(Voxel* voxel, int index) {
		m_pVoxels[index] = voxel;
	}
	inline Voxel* GetVoxelFromPosition(VoxelPosition vp);
	/**Returns the chunk position. Coordinates in chunk coordinates**/
	inline ChunkPosition GetPos() {
		return m_chunkPos;
	}
	inline bool MeshBuilt() {
		return m_meshBuilt;
	}
	inline bool VoxelsGenerated() {
		return m_voxelsGenerated;
	}
private:
	Chunk(const Chunk& other);
	void operator=(const Chunk& other);

	ChunkPosition m_chunkPos;
	Voxel** m_pVoxels;
	Mesh* m_pChunkMesh;
	World* m_pWorld;

	bool m_voxelsGenerated;
	bool m_meshBuilt;
};

#endif