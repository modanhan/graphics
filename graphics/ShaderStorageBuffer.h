#pragma once

#include <memory>

#include <glad/glad.h>
#include <glfw/glfw3.h>

#define GL_SHADER_STORAGE_BUFFER 0x90D2
#define GL_SHADER_STORAGE_BUFFER_BINDING 0x90D3
#define GL_SHADER_STORAGE_BUFFER_START 0x90D4
#define GL_SHADER_STORAGE_BUFFER_SIZE 0x90D5
#define GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS 0x90D6
#define GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS 0x90D7
#define GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS 0x90D8
#define GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS 0x90D9
#define GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS 0x90DA
#define GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS 0x90DB
#define GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS 0x90DC
#define GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS 0x90DD
#define GL_MAX_SHADER_STORAGE_BLOCK_SIZE 0x90DE
#define GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT 0x90DF

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
};

