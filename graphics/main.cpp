
#include <vector>
#include <iostream>
#include <iomanip>
#include <limits>

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "GraphicProgram.h"
#include "VertexArray.h"
#include "Texture.h"
#include "Camera.h"
#include "Geometry.h"
#include "ShaderStorageBuffer.h"

constexpr int WIDTH = 1280;
constexpr int HEIGHT = 720;

int main() {
	auto window = Window::Create(WIDTH, HEIGHT);
	auto vertexShader = Shader::Create(GL_VERTEX_SHADER, "shaders/vertex.glsl");
	auto fragmentShader = Shader::Create(GL_FRAGMENT_SHADER, "shaders/raytracing_fragment.glsl");
	auto program = GraphicProgram::Create(*vertexShader, *fragmentShader);

	std::vector<GLfloat> vertexBufferData = {
		-1, -1, 1, -1, 1, 1, 1, 1, -1, 1, -1, -1
	};

	std::vector<GLfloat> uvBufferData = {
		0, 0, 1, 0, 1, 1, 1, 1, 0, 1, 0, 0
	};

	auto vertexArray = VertexArray::Builder()
		.addBuffer(0, 2, vertexBufferData)
		.addBuffer(1, 2, uvBufferData)
		.build(6);

	using namespace RayTracing;
	using namespace glm;
	Camera camera(WIDTH, HEIGHT, 0.29);
	auto cameraSsbo = ShaderStorageBuffer::Create(sizeof(Camera), &camera, GL_DYNAMIC_COPY, 0);
	std::vector<SphereGeometry> spheres;
	spheres.push_back(SphereGeometry(vec3(0, 0, -10), 1));
	spheres.push_back(SphereGeometry(vec3(3, 1, -12), 1));
	spheres.push_back(SphereGeometry(vec3(-3, 0, -12), 1));
	auto spheresSsbo = ShaderStorageBuffer::Create(sizeof(spheres[0]) * spheres.size(), spheres.data(), GL_DYNAMIC_COPY, 1);
	printf("%d\n", sizeof(spheres[0]) * spheres.size());
	printf("%d\n", sizeof(vec3));
	printf("%d\n", sizeof(float));

	while (!window->shouldClose()) {
		program->clear();
		program->start();

		spheresSsbo->bind();
		vertexArray->render();

		program->finish();
		window->swap();
	}
	return 0;
}