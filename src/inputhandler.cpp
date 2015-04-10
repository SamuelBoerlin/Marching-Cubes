//Author: Samuel Börlin

#include "inputhandler.hpp"
#include "SDL2\SDL.h"
#include "engine.hpp"
#include <iostream>
#include "simplemc.hpp"

bool m_left = false, m_right = false, m_forw = false, m_backw = false, m_up = false, m_down = false, m_speed = false, m_lookat = false, m_wiremesh = false, m_mouseleft = false, m_mouseright = false;
SimpleMC simpleMC2;

InputHandler::InputHandler(Engine* engine) {
	m_pEngine = engine;
}

void InputHandler::Update() {
	SDL_SetRelativeMouseMode(SDL_TRUE);
	Poll();

	glm::vec3 strafeVec = -glm::normalize(glm::cross(m_pEngine->GetCamera()->GetUpVec(), m_pEngine->GetCamera()->GetForwardVec()));
	glm::vec3 forwardVec = glm::normalize(glm::vec3(m_pEngine->GetCamera()->GetForwardVec().x, 0, m_pEngine->GetCamera()->GetForwardVec().z));
	glm::vec3 upVec = glm::vec3(0, 1, 0);

	float speed = !m_speed ? 0.05f : 2.0f;

	if(m_left) {
		m_pEngine->GetCamera()->SetPos(m_pEngine->GetCamera()->GetPos() + strafeVec * -speed);
	}
	if(m_right) {
		m_pEngine->GetCamera()->SetPos(m_pEngine->GetCamera()->GetPos() + strafeVec * speed);
	}
	if(m_forw) {
		m_pEngine->GetCamera()->SetPos(m_pEngine->GetCamera()->GetPos() + forwardVec * speed);
	}
	if(m_backw) {
		m_pEngine->GetCamera()->SetPos(m_pEngine->GetCamera()->GetPos() + forwardVec * -speed);
	}
	if(m_up) {
		m_pEngine->GetCamera()->SetPos(m_pEngine->GetCamera()->GetPos() + upVec * speed);
	}
	if(m_down) {
		m_pEngine->GetCamera()->SetPos(m_pEngine->GetCamera()->GetPos() + upVec * -speed);
	}
	if(m_lookat) {
		m_pEngine->GetCamera()->LookAt(glm::vec3(0, 0, 0));
	}
	if(m_wiremesh) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	} else {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if(m_mouseleft) {
		int x = 0, y = 0, z = 0;
		float step = 0.1f;
		int iterations = 500;
		int radius = 5;

		if(!Engine::FIXEDBRUSH) {
			if(m_pEngine->RayTraceSphere(x, y, z, Engine::BRUSHSIZE / 1.5f, iterations, step, Engine::ISOLEVEL)) {
				//RayTrace hit solid block (density >= isoLevel)
				//simpleMC.IncrDensitySphere(m_pEngine->GetData(), x, y, z, Engine::SX, Engine::SY, Engine::SZ, 3, 0.1f, 0.0f, 2.0f);
				//simpleMC2.SetDensityCube(m_pEngine->GetData(), x-2, y-2, z-2, 4, 4, 4, Engine::SX, Engine::SY, Engine::SZ, 1.0f);
				if(!m_speed) {
					simpleMC2.IncrDensitySphere(x, y, z, Engine::BRUSHSIZE, 0.02f, 0.0f, 1.0f, m_pEngine->m_pWorld);
				} else {
					simpleMC2.SmoothDensitySphere(x, y, z, Engine::BRUSHSIZE, Engine::ISOLEVEL, m_pEngine->m_pWorld);
				}
				//simpleMC2.SetDensityCube(x, y, z, 6, 6, 6, 1.0f, m_pEngine->m_pWorld);
				std::cout << "Rebuilding" << std::endl;
				Chunk* pChunk = m_pEngine->m_pWorld->GetChunkWorldCoords(x, y, z);
				if(pChunk != nullptr) {
					pChunk->BuildMesh();
					pChunk->RebuildNeighbours();
				}
				//m_pEngine->Rebuild();
			}
		} else {
			glm::vec3 brushPos = m_pEngine->GetCamera()->GetPos() + m_pEngine->GetCamera()->GetForwardVec() * Engine::BRUSHDISTANCE;
			if(!m_speed) {
				simpleMC2.IncrDensitySphere((int)brushPos.x, (int)brushPos.y, (int)brushPos.z, Engine::BRUSHSIZE, 0.02f, 0.0f, 1.0f, m_pEngine->m_pWorld);
			} else {
				simpleMC2.SmoothDensitySphere((int)brushPos.x, (int)brushPos.y, (int)brushPos.z, Engine::BRUSHSIZE, Engine::ISOLEVEL, m_pEngine->m_pWorld);
			}
			std::cout << "Rebuilding" << std::endl;
			Chunk* pChunk = m_pEngine->m_pWorld->GetChunkWorldCoords((int)brushPos.x, (int)brushPos.y, (int)brushPos.z);
			if(pChunk != nullptr) {
				pChunk->BuildMesh();
				pChunk->RebuildNeighbours();
			}
			//m_pEngine->Rebuild();
		}
	}
	if(m_mouseright) {
		int x = 0, y = 0, z = 0;
		float step = 0.1f;
		int iterations = 500;
		int radius = 5;

		if(!Engine::FIXEDBRUSH) {
			if(m_pEngine->RayTraceSphere(x, y, z, Engine::BRUSHSIZE / 1.5f, iterations, step, Engine::ISOLEVEL)) {
				//RayTrace hit solid block (density >= isoLevel)
				simpleMC2.IncrDensitySphere(x, y, z, Engine::BRUSHSIZE, -0.02f, 0.0f, 1.0f, m_pEngine->m_pWorld);
				std::cout << "Rebuilding" << std::endl;
				Chunk* pChunk = m_pEngine->m_pWorld->GetChunkWorldCoords(x, y, z);
				if(pChunk != nullptr) {
					pChunk->BuildMesh();
					pChunk->RebuildNeighbours();
				}
				//m_pEngine->Rebuild();
			}
		} else {
			glm::vec3 brushPos = m_pEngine->GetCamera()->GetPos() + m_pEngine->GetCamera()->GetForwardVec() * Engine::BRUSHDISTANCE;
			simpleMC2.IncrDensitySphere((int)brushPos.x, (int)brushPos.y, (int)brushPos.z, Engine::BRUSHSIZE, -0.02f, 0.0f, 1.0f, m_pEngine->m_pWorld);
			std::cout << "Rebuilding" << std::endl;
			Chunk* pChunk = m_pEngine->m_pWorld->GetChunkWorldCoords((int)brushPos.x, (int)brushPos.y, (int)brushPos.z);
			if(pChunk != nullptr) {
				pChunk->BuildMesh();
				pChunk->RebuildNeighbours();
			}
			//m_pEngine->Rebuild();
		}
	}
	/*if(m_mouseright) {
		int x = 0, y = 0, z = 0;
		float step = 0.1f;
		int iterations = 500;
		int radius = 5;
		if(!Engine::FIXEDBRUSH) {
			if(m_pEngine->RayTraceSphere(x, y, z, Engine::BRUSHSIZE / 1.5f, iterations, step, Engine::ISOLEVEL)) {
				//RayTrace hit solid block (density >= isoLevel)
				simpleMC2.IncrDensitySphere(x, y, z, Engine::BRUSHSIZE, -0.02f, 0.0f, 1.0f, m_pEngine->m_pWorld);
				std::cout << "Rebuilding" << std::endl;
				//m_pEngine->Rebuild();
			}
		} else {
			glm::vec3 brushPos = m_pEngine->GetCamera()->GetPos() + m_pEngine->GetCamera()->GetForwardVec() * Engine::BRUSHDISTANCE;
			simpleMC2.IncrDensitySphere((int)brushPos.x, (int)brushPos.y, (int)brushPos.z, Engine::BRUSHSIZE, -0.02f, 0.0f, 1.0f, m_pEngine->m_pWorld);
			std::cout << "Rebuilding" << std::endl;
			//m_pEngine->Rebuild();
		}
	}*/
}

void InputHandler::Poll() {
	SDL_Event sdlEvent;
	while(SDL_PollEvent(&sdlEvent)) {
		switch(sdlEvent.type) {


		//Key down
		case SDL_KEYDOWN:
			switch(sdlEvent.key.keysym.sym) {

			//Close game
			case SDLK_ESCAPE:
				m_pEngine->GetDisplay()->SetClosed();
				break;

			//Movement controls
			case SDLK_a:
				m_left = true;
				break;
			case SDLK_d:
				m_right = true;
				break;
			case SDLK_s:
				m_backw = true;
				break;
			case SDLK_w:
				m_forw = true;
				break;
			case SDLK_LSHIFT:
				m_down = true;
				break;
			case SDLK_SPACE:
				m_up = true;
				break;
			case SDLK_LCTRL:
				m_speed = true;
				break;
			case SDLK_TAB:
				m_lookat = !m_lookat;
				break;
			case SDLK_m:
				m_wiremesh = !m_wiremesh;
				break;
			case SDLK_f:
				Engine::FIXEDBRUSH = !Engine::FIXEDBRUSH;
				break;
			}
			break;


		//Key up
		case SDL_KEYUP:
			switch(sdlEvent.key.keysym.sym) {

			//Movement controls
			case SDLK_a:
				m_left = false;
				break;
			case SDLK_d:
				m_right = false;
				break;
			case SDLK_s:
				m_backw = false;
				break;
			case SDLK_w:
				m_forw = false;
				break;
			case SDLK_LSHIFT:
				m_down = false;
				break;
			case SDLK_SPACE:
				m_up = false;
				break;
			case SDLK_LCTRL:
				m_speed = false;
				break;
			}
			break;


		//Mouse movement
		case SDL_MOUSEMOTION:
			if(sdlEvent.motion.xrel != 0) {
				float xrel = sdlEvent.motion.xrel;
				m_pEngine->GetCamera()->RotateYaw(-(xrel / abs(xrel) * 0.5f));
			}
			if(sdlEvent.motion.yrel != 0) {
				float yrel = sdlEvent.motion.yrel;
				m_pEngine->GetCamera()->RotatePitch((yrel / abs(yrel) * 0.5f));
			}


		//Mouse Click
		case SDL_MOUSEBUTTONDOWN:
			switch(sdlEvent.button.button) {

			case SDL_BUTTON_LEFT:
				m_mouseleft = true;
				break;
			case SDL_BUTTON_RIGHT:
				m_mouseright = true;
				break;

			}
			break;

		case SDL_MOUSEBUTTONUP:
			switch(sdlEvent.button.button) {

			case SDL_BUTTON_LEFT:
				m_mouseleft = false;
				break;
			case SDL_BUTTON_RIGHT:
				m_mouseright = false;
				break;

			}
			break;

		//Mouse Wheel
		case SDL_MOUSEWHEEL:
			if(!m_speed) {
				Engine::BRUSHSIZE += sdlEvent.wheel.y * 0.05f;
			} else {
				Engine::BRUSHDISTANCE += sdlEvent.wheel.y * 0.1f;
			}
			break;
		}

	}
}