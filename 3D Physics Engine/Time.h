#pragma once
#include <GLFW/glfw3.h>
class Time
{
public:
	Time() {
		lastTime = glfwGetTime();
		deltaTime = 0;
	}
	void update() {
		float currentTime = glfwGetTime();
		deltaTime = currentTime - lastTime;
		lastTime = currentTime;
	}
	float deltaTime;
private:
	float lastTime;

};

