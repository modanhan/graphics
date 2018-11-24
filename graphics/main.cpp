
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
	Camera camera(WIDTH, HEIGHT, 0.75);
	auto cameraSsbo = ShaderStorageBuffer::Create(sizeof(Camera), &camera, GL_DYNAMIC_COPY, 0);
	std::vector<SphereGeometry> spheres;
	spheres.push_back(SphereGeometry(vec3(0, 0, -10), 1));
	spheres.push_back(SphereGeometry(vec3(3, 1, -12), 1));
	auto spheresSsbo = ShaderStorageBuffer::Create(sizeof(spheres[0]) * spheres.size(), spheres.data(), GL_DYNAMIC_COPY, 1);
	printf("%d\n", sizeof(SphereGeometry));
	printf("%d\n", sizeof(vec3));
	printf("%d\n", sizeof(float));

	std::vector<GLubyte> image(WIDTH * HEIGHT * 4, 0);
	if(0){ // ray tracing
		using namespace RayTracing;
		using namespace glm;

		std::vector<float> depth(WIDTH * HEIGHT, std::numeric_limits<float>::max());

		std::vector<std::unique_ptr<Geometry>> geometries;
		geometries.push_back(std::make_unique<SphereGeometry>(vec3(0, 0, -10), 1));
	//	geometries.push_back(std::make_unique<SphereGeometry>(vec3(3, 0, -10), 1));
	//	geometries.push_back(std::make_unique<SphereGeometry>(vec3(3, 3, -10), 1));
		geometries.push_back(std::make_unique<TriangleGeometry>(vec3(-5, -1, -5), vec3(5, -1, -5), vec3(5, -1, -20)));
	//	geometries.push_back(std::make_unique<TriangleGeometry>(vec3(3, -1, -13), vec3(-3, -1, -13), vec3(-3, -1, -7)));

		std::vector<GLubyte> rs(geometries.size());
		std::vector<GLubyte> gs(geometries.size());
		std::vector<GLubyte> bs(geometries.size());
		for (int g = 0; g < geometries.size(); ++g) {
			rs[g] = rand();
			gs[g] = rand();
			bs[g] = rand();
		}
		std::cout.precision(3);
		for (int j = 0; j < HEIGHT; ++j) {
			for (int i = 0; i < WIDTH; ++i) {
				for (int g = 0; g < geometries.size(); ++g) {
					auto& geometry = geometries[g];
					auto hit = geometry->hit(camera.ray(i, j));
					if (hit && hit->length < depth[i + j * WIDTH]) {
						depth[i + j * WIDTH] = hit->length;
						image[(i + j * WIDTH) * 4 + 0] = rs[g];
						image[(i + j * WIDTH) * 4 + 1] = gs[g];
						image[(i + j * WIDTH) * 4 + 2] = bs[g];
					}
				}
				image[(i + j * WIDTH) * 4 + 3] = 255;
			}
			std::cout << std::setw(5) << j * 100.0 / HEIGHT << "%\r" << std::flush;
		}
		std::cout << "100.0%\r" << std::flush;
	}

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