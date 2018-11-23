#include "VertexArray.h"

std::unique_ptr<VertexBuffer> VertexBuffer::Create(const std::vector<GLfloat>& data) {
	auto buffer = std::unique_ptr<VertexBuffer>(new VertexBuffer());
	glGenBuffers(1, &buffer->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, buffer->vbo);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(data[0]), data.data(), GL_STATIC_DRAW);
	return std::move(buffer);
}

VertexBuffer::~VertexBuffer() {
	glDeleteBuffers(1, &vbo);
}

VertexArray::~VertexArray() {
	glDeleteVertexArrays(1, &vao);
}

int VertexArray::render() {
	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, elementCount);
	return 0;
}
