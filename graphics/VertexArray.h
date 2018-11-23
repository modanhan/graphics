#pragma once

#include <memory>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <map>
#include <vector>

class VertexBuffer {
	VertexBuffer() = default;
public:
	GLuint vbo;
	static std::unique_ptr<VertexBuffer> Create(const std::vector<GLfloat>& data);
	~VertexBuffer();
};

class VertexArray {
	VertexArray() = default;
	GLuint vao = 0;
	int elementCount = 0;
	std::map<GLuint, std::unique_ptr<VertexBuffer>> buffers;
public:
	~VertexArray();

	class Builder {
		std::unique_ptr<VertexArray> vertexArray;
	public:
		Builder() {
			vertexArray = std::unique_ptr<VertexArray>(new VertexArray());
			glGenVertexArrays(1, &vertexArray->vao);
			glBindVertexArray(vertexArray->vao);
		}

		Builder& addBuffer(int index, int size, const std::vector<GLfloat>& data) {
			vertexArray->buffers.emplace(index, VertexBuffer::Create(data));
			glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, 0, 0);
			glEnableVertexAttribArray(index);
			return *this;
		}

		std::unique_ptr<VertexArray> build(int elementCount) {
			vertexArray->elementCount = elementCount;
			glBindVertexArray(0);
			return std::move(vertexArray);
		}

		~Builder() {
			vertexArray.reset();
		}

	};

	int render();
};

