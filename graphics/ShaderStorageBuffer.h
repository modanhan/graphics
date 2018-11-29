#pragma once

#include <memory>

#include <glad/glad.h>
#include <glfw/glfw3.h>

class ShaderStorageBuffer
{
	GLuint ssbo;
	ShaderStorageBuffer() = default;
public:
	static std::unique_ptr<ShaderStorageBuffer> Create(GLsizeiptr size, const void* data, GLuint usage, GLuint index) {
		auto ssbo = std::unique_ptr<ShaderStorageBuffer>(new ShaderStorageBuffer());
		glGenBuffers(1, &ssbo->ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo->ssbo);
		glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, usage);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, index, ssbo->ssbo);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		return ssbo;
	}
	~ShaderStorageBuffer() {
		glDeleteBuffers(1, &ssbo);
	}
	int bind() {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
		return 0;
	}
};

