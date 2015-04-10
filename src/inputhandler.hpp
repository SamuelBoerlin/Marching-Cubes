//Author: Samuel Börlin

#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

class Engine;

class InputHandler {
public:
	InputHandler(Engine* engine);
	~InputHandler();
	void Update();

private:
	InputHandler(const InputHandler& other);
	void operator=(const InputHandler& other);

	void Poll();
	Engine* m_pEngine;
};

#endif