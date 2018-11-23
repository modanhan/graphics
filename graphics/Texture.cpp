#include "Texture.h"


Texture::~Texture()
{
	glDeleteTextures(1, &texture);
}

std::unique_ptr<Texture> Texture::Create(GLsizei width, GLsizei height, const std::vector<uint8_t> &image) {
	auto texture = std::unique_ptr<Texture>(new Texture());

	glGenTextures(1, &texture->texture);
	glBindTexture(GL_TEXTURE_2D, texture->texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data());
	glBindTexture(GL_TEXTURE_2D, 0);
	// return nullptr if glGetError
	return std::move(texture);
}

int Texture::activate(int target) {
	glActiveTexture(GL_TEXTURE0 + target);
	glBindTexture(GL_TEXTURE_2D, texture);
	// return -1 if glGetError
	return 0;
}