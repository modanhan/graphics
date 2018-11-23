
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
		Camera camera(1280, 720, 60);
		SphereGeometry sphere;
		sphere.position = vec3(0, 0, -10);
		sphere.radius = 1.0;
		for (int j = 0; j < 720; ++j) {
			for (int i = 0; i < 1280; ++i) {
				if (sphere.hit(camera.ray(i, j))) {
					image[(i + j * 1280) * 4 + 0] = 255;
					image[(i + j * 1280) * 4 + 1] = 255;
					image[(i + j * 1280) * 4 + 2] = 255;
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