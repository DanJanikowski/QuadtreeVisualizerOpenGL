#pragma once

#include <vector>

#include <glad/glad.h>

#include "globals.h"


// Class to encapsulate all mesh data
// VBO, EBO, and VAO as well as the Shapes array
// to declutter the functionality of the Environment class
class RenderObject {
public:
	RenderObject();
	RenderObject(bool requireIndices_);
	~RenderObject();

	void activate();
	void deactivate();

	// For packing index-independent data (GL_POINTS, GL_TRIANGLES)
	void pack(const void* vertices, GLsizeiptr vByteSize, bool repack);
	// For packing index-dependent data (i.e. GL_LINE_LOOP, GL_LINES, ...)
	void pack(const void* vertices, GLsizeiptr vByteSize, const void* indices, GLsizeiptr iByteSize, bool repack);

	// Draw index-independent data
	void drawArrays(GLenum mode, GLsizei count);
	// Draw index-dependent data
	void drawElements(GLenum mode, GLsizei count, bool primRestart);

private:
	bool requireIndices;
	GLuint VAO_ID, VBO_ID, EBO_ID;
};