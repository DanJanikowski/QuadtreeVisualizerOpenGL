#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>

class Shader {
public:
	GLuint rasterID;
	//GLuint computeID;

	Shader(const char* vertexFile, const char* fragmentFile);
	~Shader();

	void activateDefaultShader();

	void deleteShaders();
};