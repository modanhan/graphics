
#include <vector>
#include <iostream>
#include <iomanip>
#include <limits>
#include <chrono>

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "GraphicProgram.h"
#include "VertexArray.h"
#include "Texture.h"
#include "Camera.h"
#include "Geometry.h"
#include "ShaderStorageBuffer.h"
#include "FrameBuffer.h"

using namespace RayTracing;
using namespace glm;

constexpr int WIDTH = 1280;
constexpr int HEIGHT = 720;

int main() {
	auto window = Window::Create(WIDTH, HEIGHT);
	auto rt_vertexShader = Shader::Create(GL_VERTEX_SHADER, "shaders/vertex.glsl");
	auto rt_fragmentShader = Shader::Create(GL_FRAGMENT_SHADER, "shaders/fragment-rt.glsl");
	auto rt_program = GraphicProgram::Create(*rt_vertexShader, *rt_fragmentShader);

	auto rt_fbo = FrameBuffer::Builder()
		.addColorAttachment(0, Texture::CreateRGBA(WIDTH, HEIGHT, 0))
		.build();

	auto msaa_vertexShader = Shader::Create(GL_VERTEX_SHADER, "shaders/vertex-uv.glsl");
	auto msaa_fragmentShader = Shader::Create(GL_FRAGMENT_SHADER, "shaders/fragment-rt-adaptive-msaa.glsl");
	auto msaa_program = GraphicProgram::Create(*msaa_vertexShader, *msaa_fragmentShader);

	auto msaa_fbo = FrameBuffer::Builder()
		.addColorAttachment(0, Texture::CreateRGBA(WIDTH, HEIGHT, 0))
		.build();

	auto post_vertexShader = Shader::Create(GL_VERTEX_SHADER, "shaders/vertex-uv.glsl");
	auto post_fragmentShader = Shader::Create(GL_FRAGMENT_SHADER, "shaders/fragment-denoise.glsl");
	auto post_program = GraphicProgram::Create(*post_vertexShader, *post_fragmentShader);

	std::vector<GLfloat> vertexBufferData = {
		-1, -1, 1, -1, 1, 1, 1, 1, -1, 1, -1, -1
	};
	std::vector<GLfloat> vertexUVBufferData = {
		0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0
	};

	auto rt_vertexArray = VertexArray::Builder()
		.addBuffer(0, 2, vertexBufferData)
		.build(6);
	auto postVertexArray = VertexArray::Builder()
		.addBuffer(0, 2, vertexBufferData)
		.addBuffer(1, 2, vertexUVBufferData)
		.build(6);

	Camera camera(WIDTH, HEIGHT, 1);
	auto cameraSsbo = ShaderStorageBuffer::Create(sizeof(Camera), &camera, GL_DYNAMIC_COPY, 0);

	std::vector<SphereGeometry> spheres;
	spheres.push_back(SphereGeometry(vec3(0, 0, -7), 1));
	spheres.push_back(SphereGeometry(vec3(3, 2, -15), 3));
	spheres.push_back(SphereGeometry(vec3(-2, 0, -8), 1));
	auto spheresSsbo = ShaderStorageBuffer::Create(sizeof(spheres[0]) * spheres.size(), spheres.data(), GL_DYNAMIC_COPY, 1);

	std::vector<TriangleGeometry> triangles;
	triangles.push_back(TriangleGeometry(vec3(-50, -1, 5), vec3(50, -1, 5), vec3(50, -1, -200)));
	triangles.push_back(TriangleGeometry(vec3(50, -1, -200), vec3(-50, -1, -200), vec3(-50, -1, 5)));
	auto trianglessSsbo = ShaderStorageBuffer::Create(sizeof(triangles[0]) * triangles.size(), triangles.data(), GL_DYNAMIC_COPY, 2);

	rt_fbo->bind(); {
		rt_program->clear();
		rt_program->start();

		spheresSsbo->bind();
		trianglessSsbo->bind();
		rt_vertexArray->render();

		rt_program->finish();
	} FrameBuffer::unbind();

	msaa_fbo->bind(); {
		post_program->clear();
		post_program->start();
		rt_fbo->activate(0, 0);
		postVertexArray->render();
		post_program->finish();
	} FrameBuffer::unbind();

	{
		msaa_program->clear();
		msaa_program->start();

		spheresSsbo->bind();
		trianglessSsbo->bind();
		msaa_fbo->activate(0, 0);
		postVertexArray->render();

		msaa_program->finish();
	}

	auto timePoint = std::chrono::high_resolution_clock::now();
	window->swap();
	auto duration = std::chrono::high_resolution_clock::now() - timePoint;
	printf("us:\t%u\n", std::chrono::duration_cast<std::chrono::microseconds>(duration).count());

	while (!window->shouldClose()) {
		glfwPollEvents();
	}

	return 0;
}