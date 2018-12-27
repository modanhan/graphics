#pragma once

#include <memory>
#include <map>
#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "Texture.h"

class FrameBuffer
{
	FrameBuffer() = default;
	GLuint fbo = 0;
	std::map<int, std::unique_ptr<Texture>> colorAttachments;
public:
	int bind() const;
	static int unbind() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		return 0;
	}
	int activate(int colorAttachment, int target) const {
		return colorAttachments.at(colorAttachment)->activate(target);
	}

	class Builder {
		std::unique_ptr<FrameBuffer> frameBuffer;
	public:
		Builder() {
			frameBuffer = std::unique_ptr<FrameBuffer>(new FrameBuffer());
			glGenFramebuffers(1, &frameBuffer->fbo);
			glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->fbo);
		}

		Builder& addColorAttachment(int colorAttachment, std::unique_ptr<Texture> texture) {
			frameBuffer->colorAttachments.emplace(colorAttachment, std::move(texture));
			glBindTexture(GL_TEXTURE_2D, *(frameBuffer->colorAttachments.at(colorAttachment)));
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttachment, GL_TEXTURE_2D,
				*(frameBuffer->colorAttachments.at(colorAttachment)), 0);
			return *this;
		}

		std::unique_ptr<FrameBuffer> build() {
			std::vector<GLenum> drawBuffers;
			for (auto& p : frameBuffer->colorAttachments) {
				drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + p.first);
			}
			glDrawBuffers(drawBuffers.size(), drawBuffers.data());
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			return std::move(frameBuffer);
		}

		~Builder() {
			frameBuffer.reset();
		}

	};
	~FrameBuffer();

	operator GLuint() const {
		return fbo;
	}
	static FrameBuffer null;
};

