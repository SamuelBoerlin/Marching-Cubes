//Author: Benny Bobaganoosh

#include "display.hpp"
#include <iostream>
#include <GL\glew.h>
#include <string>

Display::Display(int width, int height, const std::string& title) {
	//Init SDL
	SDL_Init(SDL_INIT_EVERYTHING);

	//Set red bits
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	//Set green bits
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	//Set blue bits
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	//Set alpha bits
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	//Set buffer size
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	//Set depth buffer size
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	//Set double buffering
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

	//Create window with SDL
	m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_OPENGL);

	//Create GL context which makes it possible to render on the window (basically gives the GPU control over the window instead of the OS)
	m_glContext = SDL_GL_CreateContext(m_window);

	//Gets initialization status
	GLenum status = glewInit();

	//Checks if glew was initialized correctly
	if(status != GLEW_OK) {
		std::cerr << "Glew failed to initialize!" << std::endl;
	}

	m_isClosed = false;
}

Display::~Display() {
	//Delete the OpenGL context from the window
	SDL_GL_DeleteContext(m_glContext);

	//Destroy the SDL window
	SDL_DestroyWindow(m_window);

	//Quit SDL
	SDL_Quit();
}

bool Display::IsClosed() {
	return m_isClosed;
}

void Display::Update() {
	SDL_GL_SwapWindow(m_window);
}

void Display::Clear(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}