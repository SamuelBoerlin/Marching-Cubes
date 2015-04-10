//Author: Benny Bobaganoosh

#ifndef DISPLAY_H
#define DISPLAY_H

#include <string>
#include <SDL2\SDL.h>

class Display {
	public:
		/***Default constructors/destructors***/
		/*Default constructor*/
		Display(int width, int height, const std::string& title);
		/*Display destructor (virtual in case Display gets extended)*/
		virtual ~Display();

		/***Prototypes***/
		/*Clears screen and color buffer*/
		void Clear(float r, float g, float b, float a);
		/*Updates screen/Swaps buffers*/
		void Update();
		/*Returns true if closed event was found*/
		bool IsClosed();
		inline void SetClosed() { m_isClosed = true; }
		inline SDL_Window* GetWindow() { return m_window; }
	protected:
	private:
		/**Default constructors/destructors***/
		/*Display copy constructor*/
		Display(const Display& other);
		/*Display copy constructor*/
		void operator=(const Display& other);

		/**Variables***/
		/*Stores pointer to SDL Window (instead of storing the window which uses too much memory*/
		SDL_Window* m_window;
		/*Stores the GL context (pointer not needed, not much memory required)*/
		SDL_GLContext m_glContext;
		/*True if close event was called*/
		bool m_isClosed;
};

#endif