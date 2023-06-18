#pragma once

#include <iostream>
#include <string>
#include <vector>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "globals.h"
#include "shader.h"
#include "renderObject.h"
#include "quadtree.h"

class Scene {
public:
	Scene(GLFWwindow* window_);
	~Scene();

	void keyInput(int key, int scancode, int action, int mods);
	void mouseButtonInput(int button, int action, int mods);

	void drawLoop();

private:
	bool pause = false;

	// FPS variables
	double prevTime = 0.0;
	double curTime = 0.0;
	unsigned int counter = 0;
	float frameTime = 0.0001;

	void updateFPS();

	GLFWwindow* window;
	Shader *entityShader, *qtShader;
	QuadTree* qt;


	static void keyInputSetup(GLFWwindow* window, int key, int scancode, int action, int mods) {
		Scene* tempEnv = static_cast<Scene*>(glfwGetWindowUserPointer(window));
		tempEnv->keyInput(key, scancode, action, mods);
	}
	static void mouseButtonInputSetup(GLFWwindow* window, int button, int action, int mods) {
		Scene* tempEnv = static_cast<Scene*>(glfwGetWindowUserPointer(window));
		tempEnv->mouseButtonInput(button, action, mods);
	}
};