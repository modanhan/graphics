
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
#include "PostProcessing.h"
#include "hemisphere_vector_set.h"

using namespace RayTracing;
using namespace glm;

constexpr int WIDTH = 1920;
constexpr int HEIGHT = 1080;

int main() {
	auto window = Window::Create(WIDTH, HEIGHT);
	auto rt_vertexShader = Shader::Create(GL_VERTEX_SHADER, "shaders/vertex.glsl");
	auto rt_fragmentShader = Shader::Create(GL_FRAGMENT_SHADER, std::vector<std::string>{
		"shaders/fragment-rt.glsl",
		"shaders/math.glsl",
		"shaders/rt-data.glsl",
		"shaders/rt-algorithm.glsl"
		});
	auto rt_program = Program::CreateGraphic(*rt_vertexShader, *rt_fragmentShader);

	auto rt_fbo = FrameBuffer::Builder()
		.addColorAttachment(0, Texture::CreateHDR(WIDTH, HEIGHT, 0))
		.build();

	auto tonemap_fbo = FrameBuffer::Builder()
		.addColorAttachment(0, Texture::CreateRGBA(WIDTH, HEIGHT, 0))
		.build();

	auto bloom = BloomPostProcessing::Create(WIDTH, HEIGHT);
	auto tonemapping = ToneMappingPostProcessing::Create(WIDTH, HEIGHT);

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
	spheres.push_back(SphereGeometry(vec3(-2, 0.5, -10), 1.5));
	spheres.push_back(SphereGeometry(vec3(0, 0, -12), 1));
	spheres.back().emission = vec3(0.15, 0.5, 0.925) * 24.5f;
	spheres.push_back(SphereGeometry(vec3(3.5, 1, -8), 2));
	spheres.push_back(SphereGeometry(vec3(2, -.75, -7), 0.25));
//	spheres.back().emission = vec3(0.75, 0.75, 0.125) * 12.5f;

	auto spheresSsbo = ShaderStorageBuffer::Create(sizeof(spheres[0]) * spheres.size(), spheres.data(), GL_DYNAMIC_COPY, 1);

	std::vector<TriangleGeometry> triangles;
	triangles.push_back(TriangleGeometry(vec3(-15, -1, -0), vec3(15, -1, -0), vec3(15, -1, -15)));
	triangles.push_back(TriangleGeometry(vec3(15, -1, -15), vec3(-15, -1, -15), vec3(-15, -1, -0)));
	auto trianglessSsbo = ShaderStorageBuffer::Create(sizeof(triangles[0]) * triangles.size(), triangles.data(), GL_DYNAMIC_COPY, 2);

	auto hemisphere_vectors = hemisphere_halton(1 << 8);
	auto ray_vec3sSsbo = ShaderStorageBuffer::Create(
		sizeof(hemisphere_vectors[0]) * hemisphere_vectors.size(), hemisphere_vectors.data(), GL_DYNAMIC_COPY, 3
	);

	auto ibl = Texture::SBTCreateHDR("Resources/Newport_Loft_Ref.hdr");
	if (!ibl) return -1;

	bloom->bind(); {
		rt_program->clear();
		rt_program->start();

		spheresSsbo->bind();
		trianglessSsbo->bind();
		ibl->activate(4);
		rt_vertexArray->render();

		rt_program->finish();
	} FrameBuffer::unbind();

	bloom->use(*(tonemapping->frameBuffer));
	tonemapping->use(FrameBuffer::null);

	window->swap();

	while (!window->shouldClose()) {
		glfwPollEvents();
	}

	return 0;
}