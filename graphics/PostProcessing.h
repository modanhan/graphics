#pragma once
#include "GraphicProgram.h"

#include <memory>
#include <vector>
#include "VertexArray.h"
#include "FrameBuffer.h"

class PostProcessing
{
protected:
	std::unique_ptr<FrameBuffer> frameBuffer;
public:
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
	std::unique_ptr<Shader> vertexShader;
	std::unique_ptr<Shader> fragmentShader;
	std::unique_ptr<GraphicProgram> program;
	std::unique_ptr<VertexArray> postVertexArray;
public:
	static std::unique_ptr<BloomPostProcessing> Create(GLsizei width, GLsizei height) {
		auto bloom = std::unique_ptr<BloomPostProcessing>(new BloomPostProcessing);
		bloom->vertexShader = Shader::Create(GL_VERTEX_SHADER, "shaders/vertex-uv.glsl");
		bloom->fragmentShader = Shader::Create(GL_FRAGMENT_SHADER, std::vector<std::string>{
			"shaders/fragment-gaussian.glsl"
		});
		bloom->program = GraphicProgram::Create(*bloom->vertexShader, *bloom->fragmentShader);
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
		return bloom;
	}

	void use() override {
		program->clear();
		program->start();
		frameBuffer->activate(0, 0);
		postVertexArray->render();
		program->finish();
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