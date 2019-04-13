#pragma once

#include <memory>
#include <vector>
#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <string>

class Texture
{
	Texture() = default;
	GLuint texture;
public:

	~Texture();

	static std::unique_ptr<Texture> CreateRGBA(GLsizei width, GLsizei height, const void* data);
	static std::unique_ptr<Texture> CreateFloat(GLsizei width, GLsizei height, const void* data);
	static std::unique_ptr<Texture> CreateHDR(GLsizei width, GLsizei height, const void* data);
	static std::unique_ptr<Texture> SBTCreateHDR(std::string filename);
	int activate(int target);

	operator GLuint() const {
		return texture;
	}
};

