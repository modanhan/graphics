
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
	auto rt_fragmentShader = Shader::Create(GL_FRAGMENT_SHADER, "shaders/fragment-raytracing.glsl");
	auto rt_program = GraphicProgram::Create(*rt_vertexShader, *rt_fragmentShader);
	auto rt_fbo = FrameBuffer::Create(WIDTH, HEIGHT);

	auto vertexShader = Shader::Create(GL_VERTEX_SHADER, "shaders/vertex-uv.glsl");
	auto fragmentShader = Shader::Create(GL_FRAGMENT_SHADER, "shaders/fragment-denoise.glsl");
	auto program = GraphicProgram::Create(*vertexShader, *fragmentShader);

	std::vector<GLfloat> vertexBufferData = {
		-1, -1, 1, -1, 1, 1, 1, 1, -1, 1, -1, -1
	};
	std::vector<GLfloat> vertexUVBufferData = {
		0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0
	};

	auto rt_vertexArray = VertexArray::Builder()
		.addBuffer(0, 2, vertexBufferData)
		.build(6);
	auto postprocessVertexArray = VertexArray::Builder()
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

	auto beforeRenderTimePoint = std::chrono::high_resolution_clock::now();
	rt_fbo->bind(); {
		rt_program->clear();
		rt_program->start();

		spheresSsbo->bind();
		trianglessSsbo->bind();
		rt_vertexArray->render();

		rt_program->finish();
	} FrameBuffer::unbind();
	auto rayTraceDuration = std::chrono::high_resolution_clock::now() - beforeRenderTimePoint;
	beforeRenderTimePoint = std::chrono::high_resolution_clock::now();
	{
		program->clear();
		program->start();
		postprocessVertexArray->render();
		program->finish();
	}
	auto denoiseDuration = std::chrono::high_resolution_clock::now() - beforeRenderTimePoint;
	window->swap();

	printf("Raytrace us:\t%u\n", std::chrono::duration_cast<std::chrono::microseconds>(rayTraceDuration).count());
	printf("Denoise us:\t%u\n", std::chrono::duration_cast<std::chrono::microseconds>(denoiseDuration).count());

	while (!window->shouldClose()) {
		glfwPollEvents();
	}

	return 0;
}