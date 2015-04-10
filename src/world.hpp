//Author: Samuel Börlin

#ifndef WORLD_H
#define WORLD_H

#include "voxel.hpp"
#include <vector>
#include "chunk.hpp"

class World {
public:
	static const int VIEWDISTANCE = 3;
	World();
	~World();
	/**Returns the voxel. Coordinates in world coordinates**/
	Voxel* GetVoxel(int x, int y, int z);
	/**Returns the pointer of the chunk. Coordinates in chunk coordinates**/
	inline Chunk* GetChunk(int x, int y, int z);
	Chunk* GetChunkWorldCoords(int x, int y, int z);
	/**& to avoid passing a copy (return by reference instead of return by value)**/
	inline std::vector<Chunk*> &GetChunks() { return m_vpChunks; }
	/**Creates and loads a new chunk and returns the pointer of that chunk.**/
	Chunk* LoadChunk(Chunk::ChunkPosition chunkPosition);
	/**Unloads a chunk and returns true if successful.**/
	bool UnloadChunk(Chunk::ChunkPosition chunkPosition);
	void RenderChunks();
	void RebuildChunkMeshes();
	void UpdateQueues();
private:
	World(const World& other);
	void operator=(const World& other);

	int m_meshPerFrame;
	int m_generatePerFrame;

	void UpdateGenerateQueue();
	void UpdateMeshQueue();
	void UpdateRenderQueue();

	std::vector<Chunk*> m_vpChunks;
	std::vector<Chunk*> m_vpChunksToRender;
	std::vector<Chunk*> m_vpChunksToGenerate;
	std::vector<Chunk*> m_vpChunksToMesh;
};

#endif