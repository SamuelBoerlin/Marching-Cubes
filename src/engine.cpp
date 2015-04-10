//Author: Samuel Börlin

#include "engine.hpp"

#include <GL\glew.h>
#include <GL\GL.h>
#include <GL\GLU.h>
#include "perlinnoise.hpp"
#include "simplemc.hpp"
#include "perlinnoise2.hpp"
#include "chunk.hpp"
#include "world.hpp"
#include <algorithm>

#define WIDTH 800
#define HEIGHT 600

Engine* Engine::m_pEngineInstance;
float Engine::ISOLEVEL;
float Engine::BRUSHSIZE;
float Engine::BRUSHDISTANCE;
bool Engine::FIXEDBRUSH;
Camera* Engine::m_pCameraInstance;

struct LightSource
{
	glm::vec3 ambient_color;
	glm::vec3 diffuse_color;
	glm::vec3 specular_color;
	glm::vec3 position;
};

struct Material
{
	glm::vec3 ambient_color;
	glm::vec3 diffuse_color;
	glm::vec3 specular_color;
	float specular_shininess;
};

Material mat;
LightSource light;

Engine::Engine() {
	m_pDisplay = new Display(WIDTH, HEIGHT, "Marching Cubes Engine");
	m_pInputHandler = new InputHandler(this);
	m_pWorldTransform = new Transform();

	m_pWorldShader = new Shader("./res/worldShader");
	m_pWorldShader->AddUniform("light.ambient_color", Shader::Uniform3fv);
	m_pWorldShader->AddUniform("light.diffuse_color", Shader::Uniform3fv);
	m_pWorldShader->AddUniform("light.specular_color", Shader::Uniform3fv);
	m_pWorldShader->AddUniform("light.position", Shader::Uniform3fv);
	m_pWorldShader->AddUniform("lightPos", Shader::Uniform3fv);
	m_pWorldShader->AddUniform("material.ambient_color", Shader::Uniform3fv);
	m_pWorldShader->AddUniform("material.diffuse_color", Shader::Uniform3fv);
	m_pWorldShader->AddUniform("material.specular_color", Shader::Uniform3fv);
	m_pWorldShader->AddUniform("material.specular_shininess", Shader::Uniform1f);

	m_pEngineInstance = this;
}

Engine::~Engine() {

}

Mesh* marchingCubesMesh;
std::vector<SimpleMC::TRIANGLE> triangleVector;
PerlinNoise2 pn2;
SimpleMC simpleMC;
GLUquadricObj *pGluQuadric;

float DensityAt(int x, int y, int z) {
	return (pn2.FractalNoise3D(x/32.0f, y/32.0f, z/32.0f, 5, 4, 15) + 
			pn2.FractalNoise3D(x/32.0f, y/32.0f, z/32.0f, 10, 3, 18) + 
			pn2.FractalNoise3D(x/32.0f, y/32.0f, z/32.0f, 15, 1, 22))-
			pn2.FractalNoise3D(x/32.0f, y/32.0f, z/32.0f, 5, 5, 5);
}

void Engine::Init() {
	ISOLEVEL = 0.5f;
	BRUSHSIZE = 1.5f;
	BRUSHDISTANCE = 4.0f;
	FIXEDBRUSH = false;

	m_pWorld = new World();
	/*for(int x = -4; x < 4; x++) {
		for(int z = -4; z < 4; z++) {
			world->LoadChunk(x, 0, z);
		}
	}*/
	/*for(int x = -1; x < 1; x++) {
		for(int y = -1; y < 1; y++) {
			for(int z = -1; z < 1; z++) {
				world->LoadChunk(Chunk::ChunkPosition(x, y, z));
			}
		}
	}*/
	//world->RebuildChunkMeshes();

	pGluQuadric = gluNewQuadric();

	marchingCubesMesh = nullptr;

	m_pCamera = new Camera(glm::vec3(0, 0, -2), 70.0f, (float)WIDTH/(float)HEIGHT, 0.01f, 1000.0f);
	m_pCameraInstance = m_pCamera;
}

void Engine::Close() {

}

void Engine::Tick() { 
	//Update input and movement
	this->GetInputHandler()->Update();
}

void SetupPerspective(Camera* camera, float fov, float width, float height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fov, width/height, 0.01f, 1000.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camera->GetPos().x, camera->GetPos().y, camera->GetPos().z, camera->GetPos().x + camera->GetForwardVec().x, camera->GetPos().y + camera->GetForwardVec().y, camera->GetPos().z + camera->GetForwardVec().z, camera->GetUpVec().x, camera->GetUpVec().y, camera->GetUpVec().z);
	glColor4f(0, 0, 1, 0.25f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthMask(true);
}

void SetupOrthogonal(int x1, int y1, int x2, int y2) {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(x1, x2, y1, y2);
}

void Engine::Render() {
	//Clear color and depth buffer
	m_pDisplay->Clear(0.3f, 0.45f, 0.6f, 1.0f);

	glDepthMask(true);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glCullFace(GL_BACK);

	//Set world transform to 0, 0, 0
	m_pWorldTransform->SetPos(glm::vec3(0, 0, 0));

	//Bind and update world shader
	m_pWorldShader->Bind();
	m_pWorldShader->Update(*m_pWorldTransform, *m_pCamera);
	mat.ambient_color = glm::vec3(1, 1, 1);
	mat.diffuse_color = glm::vec3(1, 1, 1);
	mat.specular_color = glm::vec3(1, 1, 1);
	mat.specular_shininess = 15.0f;
	light.ambient_color = glm::vec3(0.5, 0.5, 0.5);
	light.diffuse_color = glm::vec3(0.5, 0.5, 0.5);
	light.specular_color = glm::vec3(0.5, 0.5, 0.5);
	light.position = glm::vec3(/*m_pCamera->GetPos().x, m_pCamera->GetPos().y, m_pCamera->GetPos().z*/ 10, 50, 20);
	m_pWorldShader->UpdateUniform("light.ambient_color", 1, glm::value_ptr(light.ambient_color));
	m_pWorldShader->UpdateUniform("light.diffuse_color", 1, glm::value_ptr(light.diffuse_color));
	m_pWorldShader->UpdateUniform("light.specular_color", 1, glm::value_ptr(light.specular_color));
	m_pWorldShader->UpdateUniform("light.position", 1, glm::value_ptr(light.position));
	m_pWorldShader->UpdateUniform("lightPos", 1, glm::value_ptr(light.position));
	m_pWorldShader->UpdateUniform("mat.ambient_color", 1, glm::value_ptr(mat.ambient_color));
	m_pWorldShader->UpdateUniform("mat.diffuse_color", 1, glm::value_ptr(mat.diffuse_color));
	m_pWorldShader->UpdateUniform("mat.specular_color", 1, glm::value_ptr(mat.specular_color));
	m_pWorldShader->UpdateUniform("mat.specular_shininess", mat.specular_shininess);

	//Load chunks within view distance
	for(int cx = -World::VIEWDISTANCE; cx < World::VIEWDISTANCE; cx++) {
		for(int cy = -World::VIEWDISTANCE; cy < World::VIEWDISTANCE; cy++) {
			for(int cz = -World::VIEWDISTANCE; cz < World::VIEWDISTANCE; cz++) {
				int chunkRelX = (int)floor(m_pCamera->GetPos().x / Chunk::CHUNKSIZEX) + cx;
				int chunkRelY = (int)floor(m_pCamera->GetPos().y / Chunk::CHUNKSIZEY) + cy;
				int chunkRelZ = (int)floor(m_pCamera->GetPos().z / Chunk::CHUNKSIZEZ) + cz;
				Chunk* pChunk = m_pWorld->GetChunk(chunkRelX, chunkRelY, chunkRelZ);
				if(pChunk == nullptr) {
					Chunk* newChunk = m_pWorld->LoadChunk(Chunk::ChunkPosition(chunkRelX, chunkRelY, chunkRelZ));
				}
			}
		}
	}

	//Update queues
	m_pWorld->UpdateQueues();

	//Unload chunks that are too far away
	for(int i = 0; i < m_pWorld->GetChunks().size(); i++) {
		Chunk* pChunk = m_pWorld->GetChunks()[i];
		if(pChunk != nullptr) {
			glm::vec3 chunkPos(pChunk->GetPos().x,pChunk->GetPos().y,pChunk->GetPos().z);
			glm::vec3 camPos((int)floor(m_pCamera->GetPos().x / Chunk::CHUNKSIZEX), (int)floor(m_pCamera->GetPos().y / Chunk::CHUNKSIZEY), (int)floor(m_pCamera->GetPos().z / Chunk::CHUNKSIZEZ));
			if(glm::distance(camPos, chunkPos) > World::VIEWDISTANCE*sqrt(2)+2) {
				m_pWorld->UnloadChunk(pChunk->GetPos());
			}
		}
	}

	//Render all loaded chunks
	m_pWorld->RenderChunks();

	//Unbind world shader
	glUseProgram(0);

	//Immediate rendering without shader
	glPushMatrix();

	SetupPerspective(m_pCamera, 70.0f, WIDTH, HEIGHT);

	glPushMatrix();
	if(!FIXEDBRUSH) {
		int x = 0, y = 0, z = 0;
		if(RayTraceSphere(x, y, z, Engine::BRUSHSIZE / 1.5f, 500, 0.1f, ISOLEVEL)) {
			glTranslated(x+0.5f, y+0.5f, z+0.5f);
			glShadeModel(GL_SMOOTH);
			gluSphere(pGluQuadric, Engine::BRUSHSIZE, 20, 20);
		}
	} else {
		glm::vec3 brushPos = m_pCamera->GetPos() + m_pCamera->GetForwardVec() * BRUSHDISTANCE;
		glTranslated(brushPos.x, brushPos.y, brushPos.z);
		glShadeModel(GL_SMOOTH);
		gluSphere(pGluQuadric, Engine::BRUSHSIZE, 20, 20);
	}
	glPopMatrix();

	/*glColor4f(0, 0, 0, 1);
	glPushMatrix();
	glTranslated(0, 40, 0);
	glShadeModel(GL_SMOOTH);
	gluSphere(pGluQuadric, 8, 20, 20);
	glTranslated(5.5, 0, 0);
	gluSphere(pGluQuadric, 8, 20, 20);
	glPopMatrix();*/


	SetupOrthogonal(0, 100, 100, 0);

	//Overlay rendering
	RenderOverlay();

	glPopMatrix();

	//Flush and swap buffers
	glFlush();
	m_pDisplay->Update();
}

void Engine::RenderOverlay() {
	//Render crosshair
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);
	glColor4f(1.0, 1.0, 1.0, 0.1);
	glBegin(GL_LINES);
	glVertex2f(50-(float)HEIGHT/1000.0f, 50);
	glVertex2f(50+(float)HEIGHT/1000.0f, 50);
	glVertex2f(50, 50-(float)WIDTH/1000.0f);
	glVertex2f(50, 50+(float)WIDTH/1000.0f);
	glEnd();
	glColor4f(1, 1, 1, 1);
	glDisable(GL_BLEND);
}