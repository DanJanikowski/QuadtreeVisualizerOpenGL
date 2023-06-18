
#include "scene.h"

#include <chrono>

Scene::Scene(GLFWwindow* window_) {
	srand(time(0));

	window = window_;

	glfwSetWindowUserPointer(window, this);
	glfwSetKeyCallback(window, keyInputSetup);
	glfwSetMouseButtonCallback(window, mouseButtonInputSetup);

	entityShader = new Shader("default.vert", "entity.frag");
	qtShader = new Shader("default.vert", "tree.frag");

	qt = new QuadTree();
}

Scene::~Scene() {
	delete entityShader;
	delete qtShader;
	delete qt;
}

void Scene::keyInput(int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch (key) {
		case GLFW_KEY_P:
			if (pause) pause = false;
			else {
				pause = true;
				qt->print1();
			}
			break;
		default:
			break;
		}
	}
}

void Scene::mouseButtonInput(int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		double x, y;
		glfwGetCursorPos(window, &x, &y);
		x = (x / globals::WINDOW_WIDTH) * 2.0 - 1.0;
		y = -((y / globals::WINDOW_HEIGHT) * 2.0 - 1.0);
		std::cout << x << " " << y << std::endl;
	}
}

void Scene::updateFPS() {
	double timeDiff;

	curTime = glfwGetTime();
	timeDiff = curTime - prevTime;
	counter++;
	if (timeDiff >= 1.0 / 30.0) {
		frameTime = timeDiff / counter;
		std::string FPS = std::to_string((1.0 / timeDiff) * counter);
		std::string ms = std::to_string(frameTime * 1000.0);
		std::string newTitle = "Test - " + FPS + " FPS / " + ms + " ms";
		glfwSetWindowTitle(window, newTitle.c_str());
		prevTime = curTime;
		counter = 0;
	}
}

void Scene::drawLoop() {
	using std::chrono::high_resolution_clock;
	using std::chrono::duration_cast;
	using std::chrono::duration;
	using std::chrono::milliseconds;


	qt->setup(100000);
	//qt->print1();

	qt->packEntities(false);
	qt->packQuadTree(false);

	// FPS variables
	double prevTime = 0.0;
	double curTime = 0.0;
	double timeDiff;
	float frameTime = 0.00001; // For time dependent movement
	unsigned int counter = 0;

	// Main drawing while loop
	float iter = 0;
	while (!glfwWindowShouldClose(window)) {
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			break;
		}

		// Compute and display FPS
		updateFPS();
		curTime = glfwGetTime();


		// Draw
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw grid
		qtShader->activateDefaultShader();
		qt->drawQuadTree();

		// Draw entities
		entityShader->activateDefaultShader();
		qt->drawEntities();

		// Updates
		if (!pause) {
			//auto t1 = high_resolution_clock::now();
			qt->updatePoints();
			//auto t2 = high_resolution_clock::now();
			//auto ms_int = duration_cast<milliseconds>(t2 - t1);
			//duration<double, std::milli> ms_double = t2 - t1;
			////std::cout << ms_int.count() << "ms\n";
			//std::cout << ms_double.count() << "ms\n";

			qt->cleanup();
			qt->packEntities(true);
			qt->packQuadTree(false);
		}


		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}