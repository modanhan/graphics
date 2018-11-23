#pragma once

#include <memory>
#include <vector>
#include <glad/glad.h>
#include <glfw/glfw3.h>

class Texture
{
	Texture() = default;
	GLuint texture;
public:

	~Texture();

	static std::unique_ptr<Texture> Create(GLsizei width, GLsizei height, const std::vector<uint8_t> &image);
	int activate(int target);
};

