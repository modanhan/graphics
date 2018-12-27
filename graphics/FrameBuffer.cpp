#include "FrameBuffer.h"

int FrameBuffer::bind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	return 0;
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &fbo);
}

FrameBuffer FrameBuffer::null = FrameBuffer();