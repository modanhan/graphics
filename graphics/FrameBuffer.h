#pragma once

#include <memory>
#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "Texture.h"

class FrameBuffer
{
	FrameBuffer() = default;
	GLuint fbo;
public:
	std::unique_ptr<Texture> texture;
	static std::unique_ptr<FrameBuffer> Create(int width, int height);
	int bind();
	static int unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return 0;
	}
	int activate(int target) {
		return texture->activate(target);
	}
	~FrameBuffer();
};

