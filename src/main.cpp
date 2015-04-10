//Author: Samuel Börlin

#include <iostream>
#include <Windows.h>

#include "engine.hpp"
#include <thread>

const int TPS = 60;

int main(int argc, char** argv) {
	const int MS_STEP_TIME = 1000 / TPS;
	
	Engine pEngine;

	pEngine.Init();

	int frames = 0;
	int ticks = 0;
	int cTime = 0;
	int totalTimeDelta = 0;
	int previousMillis = GetTickCount();

	while(!pEngine.GetDisplay()->IsClosed()) {
		//Get the start time
		int currentMillis = GetTickCount();
		int timeDelta = currentMillis - previousMillis;
		previousMillis = GetTickCount();
		totalTimeDelta += timeDelta;

		while(totalTimeDelta >= MS_STEP_TIME) {
			//Tick
			pEngine.Tick();
			totalTimeDelta -= MS_STEP_TIME;
			ticks++;
		}

		//Render
		pEngine.Render();

		//Update FPS
		frames++;
		cTime += (GetTickCount() - currentMillis);
		if(cTime >= 1000) {
			cTime = 0;
			std::cout << "FPS: " << frames << " TPS: " << ticks << std::endl;
			std::cout << "POS: " << pEngine.GetCamera()->GetPos().x << " " << pEngine.GetCamera()->GetPos().y << " " << pEngine.GetCamera()->GetPos().z << std::endl;
			std::cout << "ROT: " << pEngine.GetCamera()->GetRotationYaw() << " " << pEngine.GetCamera()->GetRotationPitch() << std::endl;
			std::cout << std::endl;
			frames = 0;
			ticks = 0;
		}
	}

	pEngine.Close();

	return 0;
}