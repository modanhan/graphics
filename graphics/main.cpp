
#include <vector>

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "GraphicProgram.h"
#include "VertexArray.h"
#include "Texture.h"

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

	std::vector<GLubyte> image(1280 * 720 * 4, 255);
	for (int i = 0; i < image.size(); i += 4) image[i] = i / 10;
	auto texture = Texture::Create(1280, 720, image);

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