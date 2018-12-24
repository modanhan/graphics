
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

constexpr int WIDTH = 1280;
constexpr int HEIGHT = 720;

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

	auto msaa_vertexShader = Shader::Create(GL_VERTEX_SHADER, "shaders/vertex-uv.glsl");
	auto msaa_fragmentShader = Shader::Create(GL_FRAGMENT_SHADER, std::vector<std::string>{
		"shaders/fragment-rt-adaptive-msaa.glsl",
		"shaders/math.glsl",
		"shaders/rt-data.glsl",
		"shaders/rt-algorithm.glsl"
	});
	auto msaa_program = Program::CreateGraphic(*msaa_vertexShader, *msaa_fragmentShader);

	auto msaa_fbo = FrameBuffer::Builder()
		.addColorAttachment(0, Texture::CreateHDR(WIDTH, HEIGHT, 0))
		.build();
	auto tonemap_fbo = FrameBuffer::Builder()
		.addColorAttachment(0, Texture::CreateRGBA(WIDTH, HEIGHT, 0))
		.build();

	auto denoise = DenoisePostProcessing::Create(WIDTH, HEIGHT);
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
	spheres.push_back(SphereGeometry(vec3(0, 0, -10), 1));
	spheres.back().emission = vec3(0.25, 0.5, 1.0) * 6.5f;
	spheres.push_back(SphereGeometry(vec3(3, 2, -12), 3));
	spheres.push_back(SphereGeometry(vec3(-2, 0, -8), 1));
	auto spheresSsbo = ShaderStorageBuffer::Create(sizeof(spheres[0]) * spheres.size(), spheres.data(), GL_DYNAMIC_COPY, 1);

	std::vector<TriangleGeometry> triangles;
	triangles.push_back(TriangleGeometry(vec3(-50, -1, 5), vec3(50, -1, 5), vec3(50, -1, -200)));
	triangles.push_back(TriangleGeometry(vec3(50, -1, -200), vec3(-50, -1, -200), vec3(-50, -1, 5)));
//	triangles.push_back(TriangleGeometry(vec3(-50, -50, -8), vec3(50, -50, -8), vec3(50, 50, -8)));
//	triangles.push_back(TriangleGeometry(vec3(50, 50, -8), vec3(-50, 50, -8), vec3(-50, -50, -8)));
	auto trianglessSsbo = ShaderStorageBuffer::Create(sizeof(triangles[0]) * triangles.size(), triangles.data(), GL_DYNAMIC_COPY, 2);

	auto ray_vec3sSsbo = ShaderStorageBuffer::Create(
		sizeof(hemisphere_vector_set[0]) * hemisphere_vector_set.size(), hemisphere_vector_set.data(), GL_DYNAMIC_COPY, 3
	);

	rt_fbo->bind(); {
		rt_program->clear();
		rt_program->start();

		spheresSsbo->bind();
		trianglessSsbo->bind();
		rt_vertexArray->render();

		rt_program->finish();
	} FrameBuffer::unbind();

	// caveat: runtime is unpredictable if image is noisy that msaa trigger on many fragments
	// this can happen with diffuse surfaces interacting with high intensity emisssion
	bloom->bind(); {
		msaa_program->clear();
		msaa_program->start();

		spheresSsbo->bind();
		trianglessSsbo->bind();
		rt_fbo->activate(0, 0);
		postVertexArray->render();

		msaa_program->finish();
	} FrameBuffer::unbind();

	{
		bloom->use(*(tonemapping->frameBuffer));
	} FrameBuffer::unbind();

	{
		tonemapping->use();
	}

	window->swap();

	while (!window->shouldClose()) {
		glfwPollEvents();
	}

	return 0;
}