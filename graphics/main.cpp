
#include <vector>

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "GraphicProgram.h"
#include "VertexArray.h"
#include "Texture.h"
#include "Camera.h"
#include "Geometry.h"


int main() {
	auto window = Window::Create(1280, 720);
	auto vertexShader = Shader::Create(GL_VERTEX_SHADER, "../shaders/vertex.glsl");
	auto fragmentShader = Shader::Create(GL_FRAGMENT_SHADER, "../shaders/fragment.glsl");
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

	std::vector<GLubyte> image(1280 * 720 * 4, 0);

	{ // ray tracing
		using namespace RayTracing;
		using namespace glm;
		Camera camera(1280, 720, 0.75);

		std::vector<std::unique_ptr<Geometry>> geometries;
		geometries.push_back(std::make_unique<SphereGeometry>(vec3(0, 0, -10), 1));
		geometries.push_back(std::make_unique<SphereGeometry>(vec3(3, 0, -10), 1));
		geometries.push_back(std::make_unique<SphereGeometry>(vec3(3, 3, -10), 1));
		geometries.push_back(std::make_unique<TriangleGeometry>(vec3(-3, -1, -7), vec3(3, -1, -7), vec3(3, -1, -13)));
		geometries.push_back(std::make_unique<TriangleGeometry>(vec3(3, -1, -13), vec3(-3, -1, -13), vec3(-3, -1, -7)));

		std::vector<GLubyte> rs(geometries.size());
		std::vector<GLubyte> gs(geometries.size());
		std::vector<GLubyte> bs(geometries.size());
		for (int g = 0; g < geometries.size(); ++g) {
			rs[g] = rand();
			gs[g] = rand();
			bs[g] = rand();
		}

		for (int j = 0; j < 720; ++j) {
			for (int i = 0; i < 1280; ++i) {
				for (int g = 0; g < geometries.size(); ++g) {
					auto& geometry = geometries[g];
					if (geometry->hit(camera.ray(i, j))) {
						image[(i + j * 1280) * 4 + 0] = rs[g];
						image[(i + j * 1280) * 4 + 1] = gs[g];
						image[(i + j * 1280) * 4 + 2] = bs[g];
					}
				}
				image[(i + j * 1280) * 4 + 3] = 255;
			}
		}
	}

	auto texture = Texture::CreateRGBA(1280, 720, image.data());

	while (!window->shouldClose()) {
		program->clear();
		program->start();

		texture->activate(0);
		vertexArray->render();

		program->finish();
		window->swap();
	}
	return 0;
}