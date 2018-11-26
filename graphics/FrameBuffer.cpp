#include "FrameBuffer.h"



std::unique_ptr<FrameBuffer> FrameBuffer::Create(int width, int height) {
	auto frameBuffer = std::unique_ptr<FrameBuffer>(new FrameBuffer());
	frameBuffer->texture = Texture::CreateRGBA(width, height, nullptr);
	glGenFramebuffers(1, &frameBuffer->fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->fbo);

	glBindTexture(GL_TEXTURE_2D, frameBuffer->texture->texture);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBuffer->texture->texture, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	return frameBuffer;
}

int FrameBuffer::bind() {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return 0;
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &fbo);
}
