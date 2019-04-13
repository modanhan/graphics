#include "Texture.h"

#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

Texture::~Texture()
{
	glDeleteTextures(1, &texture);
}

std::unique_ptr<Texture> Texture::CreateRGBA(GLsizei width, GLsizei height, const void* data) {
	auto texture = std::unique_ptr<Texture>(new Texture());

	glGenTextures(1, &texture->texture);
	glBindTexture(GL_TEXTURE_2D, texture->texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);
	// return nullptr if glGetError
	return std::move(texture);
}

std::unique_ptr<Texture> Texture::CreateFloat(GLsizei width, GLsizei height, const void* data) {
	auto texture = std::unique_ptr<Texture>(new Texture());

	glGenTextures(1, &texture->texture);
	glBindTexture(GL_TEXTURE_2D, texture->texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_FLOAT, data);
	glBindTexture(GL_TEXTURE_2D, 0);
	// return nullptr if glGetError
	return std::move(texture);
}

std::unique_ptr<Texture> Texture::CreateHDR(GLsizei width, GLsizei height, const void* data) {
	auto texture = std::unique_ptr<Texture>(new Texture());

	glGenTextures(1, &texture->texture);
	glBindTexture(GL_TEXTURE_2D, texture->texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);
	glBindTexture(GL_TEXTURE_2D, 0);
	// return nullptr if glGetError
	return std::move(texture);
}

int Texture::activate(int target) {
	glActiveTexture(GL_TEXTURE0 + target);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(target, target);
	// return -1 if glGetError
	return 0;
}

std::unique_ptr<Texture> Texture::SBTCreateHDR(string filename) {
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	float *data = stbi_loadf(filename.c_str(), &width, &height, &nrComponents, 0);
	if (!data)return nullptr;

	auto ret = CreateHDR(width, height, data);

	stbi_image_free(data);
	return ret;
}
