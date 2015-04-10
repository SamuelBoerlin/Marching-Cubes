//Author: Samuel Börlin

#ifndef ENGINE_H
#define ENGINE_H

#include "display.hpp"
#include "inputhandler.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "transform.hpp"
#include "world.hpp"

class Engine {
public:
	Engine();
	~Engine();
	void Init();
	void Close();
	void Tick();
	void Render();
	inline Display* GetDisplay() { return m_pDisplay; }
	inline InputHandler* GetInputHandler() { return m_pInputHandler; }
	inline Camera* GetCamera() { return m_pCamera; }
	inline Shader* GetWorldShader() { return m_pWorldShader; }
	inline Transform* GetWorldTransform() { return m_pWorldTransform; }
	inline static Engine* GetInstance() { return m_pEngineInstance; }
	inline float ***GetData() { return m_data; }
	bool RayTrace(int &x, int &y, int &z, int iterations, float step, float isoLevel) {
		glm::vec3 viewVec = m_pCamera->GetForwardVec();
		glm::vec3 rayTrace = glm::vec3(m_pCamera->GetPos().x, m_pCamera->GetPos().y, m_pCamera->GetPos().z);
		viewVec = glm::normalize(viewVec);
		viewVec *= step;
		for(int i = 0; i < iterations; i++) {
			rayTrace += viewVec;
			int blockX = floor(rayTrace.x);
			int blockY = floor(rayTrace.y);
			int blockZ = floor(rayTrace.z);
			//if(blockX < 0 || blockX >= Engine::SX || blockY < 0 || blockY >= SY || blockZ < 0 || blockZ >= SZ) continue;
			if(/*m_data[blockX][blockY][blockZ] >= isoLevel*/m_pWorld->GetVoxel(blockX, blockY, blockZ) != nullptr && m_pWorld->GetVoxel(blockX, blockY, blockZ)->density >= isoLevel) {
				x = blockX;
				y = blockY;
				z = blockZ;
				return true;
			}
			rayTrace += viewVec;
		}
		return false;
	}
	bool RayTraceSphere(int &x, int &y, int &z, float radius, int iterations, float step, float isoLevel) {
		glm::vec3 viewVec = m_pCamera->GetForwardVec();
		glm::vec3 rayTrace = glm::vec3(m_pCamera->GetPos().x, m_pCamera->GetPos().y, m_pCamera->GetPos().z);
		viewVec = glm::normalize(viewVec);
		viewVec *= step;
		for(int i = 0; i < iterations; i++) {
			rayTrace += viewVec;
			for(int xc = -radius - 1; xc < radius + 1; xc++) {
				for(int yc = -radius - 1; yc < radius + 1; yc++) {
					for(int zc = -radius - 1; zc < radius + 1; zc++) {
						int blockX = floor(rayTrace.x + xc);
						int blockY = floor(rayTrace.y + yc);
						int blockZ = floor(rayTrace.z + zc);
						if(glm::distance(glm::vec3(blockX, blockY, blockZ), glm::vec3(rayTrace.x, rayTrace.y, rayTrace.z)) > radius) continue;
						//if(blockX < 0 || blockX >= Engine::SX || blockY < 0 || blockY >= SY || blockZ < 0 || blockZ >= SZ) continue;
						if(/*m_data[blockX][blockY][blockZ] >= isoLevel*/m_pWorld->GetVoxel(blockX, blockY, blockZ) != nullptr && m_pWorld->GetVoxel(blockX, blockY, blockZ)->density >= isoLevel) {
							x = floor(rayTrace.x);
							y = floor(rayTrace.y);
							z = floor(rayTrace.z);
							return true;
						}
					}
				}
			}
			rayTrace += viewVec;
		}
		return false;
	}
	static const int SX = 16;
	static const int SY = 16;
	static const int SZ = 16;
	static const int GRIDSIZE = 1;
	static bool FIXEDBRUSH;
	static float BRUSHDISTANCE;
	static float ISOLEVEL;
	static float BRUSHSIZE;
	static Camera* m_pCameraInstance;
	World* m_pWorld;

private:
	Engine(const Engine& other);
	void operator=(const Engine& other);

	float ***m_data;
	void RenderOverlay();
	Display* m_pDisplay;
	InputHandler* m_pInputHandler;
	Camera* m_pCamera;
	Shader* m_pWorldShader;
	Transform* m_pWorldTransform;
	static Engine* m_pEngineInstance;
};

#endif