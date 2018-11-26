#pragma once

#include <memory>
#include <vector>
#include <glad/glad.h>
#include <glfw/glfw3.h>

class Texture
{
	Texture() = default;
public:
	GLuint texture;

	~Texture();

	static std::unique_ptr<Texture> CreateRGBA(GLsizei width, GLsizei height, const void* data);
	int activate(int target);
};

