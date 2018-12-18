#pragma once
#include "GraphicProgram.h"

#include <memory>
#include <vector>
#include "VertexArray.h"
#include "FrameBuffer.h"

class PostProcessing
{
public:
	std::unique_ptr<FrameBuffer> frameBuffer;
	PostProcessing() = default;
	~PostProcessing() = default;

	void bind() { frameBuffer->bind(); }
	virtual void use() { }
};

class DenoisePostProcessing : public PostProcessing {
private:
	std::unique_ptr<Shader> vertexShader;
	std::unique_ptr<Shader> fragmentShader;
	std::unique_ptr<GraphicProgram> program;
	std::unique_ptr<VertexArray> postVertexArray;
public:
	static std::unique_ptr<DenoisePostProcessing> Create(GLsizei width, GLsizei height) {
		auto denoise = std::unique_ptr<DenoisePostProcessing>(new DenoisePostProcessing);
		denoise->vertexShader = Shader::Create(GL_VERTEX_SHADER, "shaders/vertex-uv.glsl");
		denoise->fragmentShader = Shader::Create(GL_FRAGMENT_SHADER, std::vector<std::string>{
			"shaders/fragment-denoise.glsl"
		});
		denoise->program = GraphicProgram::Create(*denoise->vertexShader, *denoise->fragmentShader);
		denoise->frameBuffer = FrameBuffer::Builder()
			.addColorAttachment(0, Texture::CreateHDR(width, height, 0))
			.build();
		std::vector<GLfloat> vertexBufferData = {
			-1, -1, 1, -1, 1, 1, 1, 1, -1, 1, -1, -1
		};
		std::vector<GLfloat> vertexUVBufferData = {
			0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0
		};
		denoise->postVertexArray = VertexArray::Builder()
			.addBuffer(0, 2, vertexBufferData)
			.addBuffer(1, 2, vertexUVBufferData)
			.build(6);
		return denoise;
	}

	void use() override {
		program->clear();
		program->start();
		frameBuffer->activate(0, 0);
		postVertexArray->render();
		program->finish();
	}
};

class BloomPostProcessing : public PostProcessing {
private:
	GLsizei width;
	GLsizei height;
	std::unique_ptr<Shader> vertexShader;
	std::unique_ptr<Shader> fragmentShader;
	std::unique_ptr<Shader> addFragmentShader;
	std::unique_ptr<Shader> lerpFragmentShader;
	std::unique_ptr<GraphicProgram> program;
	std::unique_ptr<GraphicProgram> addProgram;
	std::unique_ptr<GraphicProgram> lerpProgram;
	std::unique_ptr<VertexArray> postVertexArray;
	std::vector<std::unique_ptr<FrameBuffer>> frameBuffers;
	std::vector<std::unique_ptr<FrameBuffer>> frameBuffersV;
public:
	static std::unique_ptr<BloomPostProcessing> Create(GLsizei width, GLsizei height) {
		auto bloom = std::unique_ptr<BloomPostProcessing>(new BloomPostProcessing);
		bloom->width = width;
		bloom->height = height;
		bloom->vertexShader = Shader::Create(GL_VERTEX_SHADER, "shaders/vertex-uv.glsl");
		bloom->fragmentShader = Shader::Create(GL_FRAGMENT_SHADER, std::vector<std::string>{
			"shaders/fragment-gaussian.glsl"
		});
		bloom->addFragmentShader = Shader::Create(GL_FRAGMENT_SHADER, std::vector<std::string>{
			"shaders/fragment-gaussian-add.glsl"
		});
		bloom->lerpFragmentShader = Shader::Create(GL_FRAGMENT_SHADER, std::vector<std::string>{
			"shaders/fragment-lerp.glsl"
		});
		bloom->program = GraphicProgram::Create(*bloom->vertexShader, *bloom->fragmentShader);
		bloom->addProgram = GraphicProgram::Create(*bloom->vertexShader, *bloom->addFragmentShader);
		bloom->lerpProgram = GraphicProgram::Create(*bloom->vertexShader, *bloom->lerpFragmentShader);
		bloom->frameBuffer = FrameBuffer::Builder()
			.addColorAttachment(0, Texture::CreateHDR(width, height, 0))
			.build();
		std::vector<GLfloat> vertexBufferData = {
			-1, -1, 1, -1, 1, 1, 1, 1, -1, 1, -1, -1
		};
		std::vector<GLfloat> vertexUVBufferData = {
			0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0
		};
		bloom->postVertexArray = VertexArray::Builder()
			.addBuffer(0, 2, vertexBufferData)
			.addBuffer(1, 2, vertexUVBufferData)
			.build(6);
		int iteration_count = 8;
		do {
			bloom->frameBuffers.push_back(FrameBuffer::Builder()
				.addColorAttachment(0, Texture::CreateHDR(width, height, 0))
				.build()
			);
			width >>= 1;
			height >>= 1;
			bloom->frameBuffersV.push_back(FrameBuffer::Builder()
				.addColorAttachment(0, Texture::CreateHDR(width, height, 0))
				.build()
			);
			if (width < 4 || height < 4) break;
			iteration_count--;
			if (!iteration_count)break;
		} while (1);
		return bloom;
	}

	void use(const FrameBuffer& externalFrameBuffer) {
		for (unsigned int i = 0; i < frameBuffers.size(); ++i) {
			frameBuffers[i]->bind();
			program->clear();
			program->start();
			glUniform2f(1, 1.0, 0.0);
			glViewport(0, 0, width >> (i), height >> (i));
			(i > 0 ? frameBuffersV[i - 1] : frameBuffer)->activate(0, 0);
			postVertexArray->render();
			program->finish();

			frameBuffersV[i]->bind();
			program->clear();
			program->start();
			glUniform2f(1, 0.0, 1.0);
			glViewport(0, 0, width >> (i + 1), height >> (i + 1));
			frameBuffers[i]->activate(0, 0);
			postVertexArray->render();
			program->finish();
		}

		for (int i = frameBuffers.size() - 2; i >= 0; --i) {
			frameBuffers[i]->bind();
			addProgram->clear();
			addProgram->start();
			glViewport(0, 0, width >> i, height >> i);
			frameBuffersV[i]->activate(0, 0);
			((i == frameBuffers.size() - 2) ? frameBuffersV[i] : frameBuffers[i + 1])->activate(0, 1);
			postVertexArray->render();
			addProgram->finish();
		}
	/*	frameBuffers.front()->bind();
		addProgram->clear();
		addProgram->start();
		glViewport(0, 0, width, height);
		frameBuffers[0]->activate(0, 0);
		frameBuffers[0]->activate(0, 1);
		postVertexArray->render();
		addProgram->finish();*/

		externalFrameBuffer.bind();
		lerpProgram->clear();
		lerpProgram->start();
		glViewport(0, 0, width, height);
		frameBuffers.front()->activate(0, 0);
		frameBuffer->activate(0, 1);
		postVertexArray->render();
		lerpProgram->finish();
	}
};

class ToneMappingPostProcessing : public PostProcessing {
private:
	std::unique_ptr<Shader> vertexShader;
	std::unique_ptr<Shader> fragmentShader;
	std::unique_ptr<GraphicProgram> program;
	std::unique_ptr<VertexArray> postVertexArray;
public:
	static std::unique_ptr<ToneMappingPostProcessing> Create(GLsizei width, GLsizei height) {
		auto toneMapping = std::unique_ptr<ToneMappingPostProcessing>(new ToneMappingPostProcessing);
		toneMapping->vertexShader = Shader::Create(GL_VERTEX_SHADER, "shaders/vertex-uv.glsl");
		toneMapping->fragmentShader = Shader::Create(GL_FRAGMENT_SHADER, std::vector<std::string>{
			"shaders/fragment-tonemapping.glsl"
		});
		toneMapping->program = GraphicProgram::Create(*toneMapping->vertexShader, *toneMapping->fragmentShader);
		toneMapping->frameBuffer = FrameBuffer::Builder()
			.addColorAttachment(0, Texture::CreateHDR(width, height, 0))
			.build();
		std::vector<GLfloat> vertexBufferData = {
			-1, -1, 1, -1, 1, 1, 1, 1, -1, 1, -1, -1
		};
		std::vector<GLfloat> vertexUVBufferData = {
			0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0
		};
		toneMapping->postVertexArray = VertexArray::Builder()
			.addBuffer(0, 2, vertexBufferData)
			.addBuffer(1, 2, vertexUVBufferData)
			.build(6);
		return toneMapping;
	}

	void use() override {
		program->clear();
		program->start();
		frameBuffer->activate(0, 0);
		postVertexArray->render();
		program->finish();
	}
};