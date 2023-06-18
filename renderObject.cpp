
#include "renderObject.h"

RenderObject::RenderObject(bool requireIndices_) {
	requireIndices = requireIndices_;

	glGenVertexArrays(1, &VAO_ID);
	glGenBuffers(1, &VBO_ID);
	if (requireIndices) glGenBuffers(1, &EBO_ID);

	glBindVertexArray(VAO_ID);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_ID);

	// Define VBO layout
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
RenderObject::~RenderObject() {
	glDeleteVertexArrays(1, &VAO_ID);
	glDeleteBuffers(1, &VBO_ID);
	if (requireIndices) glDeleteBuffers(1, &EBO_ID);
}

void RenderObject::activate() {
	glBindVertexArray(VAO_ID);
}
void RenderObject::deactivate() {
	glBindVertexArray(0);
}


void RenderObject::pack(const void* vertices, GLsizeiptr vByteSize, bool repack) {
	glBindVertexArray(VAO_ID);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_ID);
	if (repack) glBufferSubData(GL_ARRAY_BUFFER, 0, vByteSize, vertices);
	else glBufferData(GL_ARRAY_BUFFER, vByteSize, vertices, GL_DYNAMIC_DRAW);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void RenderObject::pack(const void* vertices, GLsizeiptr vByteSize, const void* indices, GLsizeiptr iByteSize, bool repack) {
	glBindVertexArray(VAO_ID);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_ID);
	if (repack) {
		glBufferSubData(GL_ARRAY_BUFFER, 0, vByteSize, vertices);
		glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, iByteSize, indices);
	}
	else {
		glBufferData(GL_ARRAY_BUFFER, vByteSize, vertices, GL_DYNAMIC_DRAW);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, iByteSize, indices, GL_STATIC_DRAW);
	}

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void RenderObject::drawArrays(GLenum mode, GLsizei count) {
	glBindVertexArray(VAO_ID);
	glDrawArrays(mode, 0, count);
	glBindVertexArray(0);
}
void RenderObject::drawElements(GLenum mode, GLsizei count, bool primRestart) {
	glBindVertexArray(VAO_ID);
	if (primRestart) {
		glEnable(GL_PRIMITIVE_RESTART);
		glPrimitiveRestartIndex(globals::PRIM_RESTART_INDEX);
		glDrawElements(mode, count, GL_UNSIGNED_INT, 0);
		glDisable(GL_PRIMITIVE_RESTART);
	}
	else glDrawElements(mode, count, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}