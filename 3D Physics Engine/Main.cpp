#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLM_ENABLE_EXPERIMENTAL

#include "ObjectRenderer.h"
#include "Camera.h"
#include "Object.h"
#include "PhysicsWorld.h"
#include "Objects.h"
#include "PlaneCollider.h"

unsigned int width = 1920;
unsigned int height = 1080;

int main() {

	glfwInit();

	// Tells GLFW what version of OPENGL is going to be used
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	//We are using the core profile, which means we have access to the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Create the window with a specified size
	GLFWwindow* window = glfwCreateWindow(width, height, "fortnite", NULL, NULL);
	if (window == NULL) {
		std::cout << "Window Creation Failed\n";
		glfwTerminate();
		return -1;
	}
	//Display Window as current context
	glfwMakeContextCurrent(window);


	//Load Glad to COnfigure OpenGL
	gladLoadGL();

	
	//Specifiy the viewport of OpenGL
	glViewport(0, 0, width, height);

	PhysicsWorld world;
	ObjectRenderer cubeRenderer(ShapeType::Cube);
	ObjectRenderer planeRenderer(ShapeType::Plane);


	double prevTime = glfwGetTime();
	
	glEnable(GL_DEPTH_TEST);

	float fov = glm::radians(45.0f);
	float aspectRatio = (float)width / (float)height;
	float nearPlane = 0.1f;
	float farPlane = 100.0f;

	cubeRenderer.SetProjection(glm::perspective(fov, aspectRatio, nearPlane, farPlane));
	planeRenderer.SetProjection(glm::perspective(fov, aspectRatio, nearPlane, farPlane));
	Camera camera(width, height, glm::vec3(0.0f, -2.0f, 10.0f));



	Object* cube2 = new Object(ShapeType::Cube, glm::vec3(0.0f, 2.0f, 0.0f), glm::vec3(0.0f, 90.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), 5.0f);
	BoxCollider* cube2Collider = new BoxCollider(cube2);
	world.AddObject(cube2);
	cube2->rb->hasGravity = true;
	
	Object* floorCube = new Object(ShapeType::Cube, glm::vec3(0.0f, -5.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(10.0f, 1.0f, 10.0f), glm::vec3(0.2f, 0.2f, 0.2f), 5.0f, false);
	BoxCollider* floorCollider = new BoxCollider(floorCube);
	world.AddObject(floorCube);

	std::vector<Object*> objects;
	objects.push_back(floorCube);
	//objects.push_back(plane);

// Random spread configuration
	int numCubes = 6;
	float planeSize = 10.0f;
	float cubeSize = 1.0f;
	float baseDropHeight = 5.0f;
	float heightStagger = 1.0f; // Additional height per cube

	srand(static_cast<unsigned>(time(0)));

	for (int i = 0; i < numCubes; i++) {
		float randomX = ((float)rand() / RAND_MAX) * planeSize - (planeSize / 2.0f);
		float randomZ = ((float)rand() / RAND_MAX) * planeSize - (planeSize / 2.0f);

		// Stagger the drop height
		float dropHeight = baseDropHeight + (i * heightStagger);

		float r = (float)rand() / RAND_MAX;
		float g = (float)rand() / RAND_MAX;
		float b = (float)rand() / RAND_MAX;

		Object* cube = new Object(
			ShapeType::Cube,
			glm::vec3(randomX, dropHeight, randomZ),
			glm::vec3(0.0f, 90.0f, 0.0f),
			glm::vec3(cubeSize, cubeSize, cubeSize),
			glm::vec3(r, g, b),
			5.0f
		);

		BoxCollider* cubeCollider = new BoxCollider(cube);
		world.AddObject(cube);
		cube->rb->hasGravity = true;
		objects.push_back(cube);
	}
	while (!glfwWindowShouldClose(window)) {
		double currentTime = glfwGetTime();
		float deltaTime = static_cast<float>(currentTime - prevTime);
		prevTime = currentTime;
		//// Handle input
		//camera.Inputs(window);
		//std::cout << "Running\n";
		//Physics
		world.Step(deltaTime);

		// Render setup
		glClearColor(0.1f, 0.67f, 0.42f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw objects
		for (Object* obj : objects) {
			if (obj->type == ShapeType::Cube) {
				obj->Draw(cubeRenderer, camera);
			}
			else if (obj->type == ShapeType::Plane) {
				obj->Draw(planeRenderer, camera);
			}
		}

		//Update Image each frame
		glfwSwapBuffers(window);

		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}