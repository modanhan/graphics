
#include <vector>

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "GraphicProgram.h"
#include "VertexArray.h"

int main() {
	auto window = Window::Create();
	auto vertexShader = Shader::Create(GL_VERTEX_SHADER, "../shaders/vertex.glsl");
	auto fragmentShader = Shader::Create(GL_FRAGMENT_SHADER, "../shaders/fragment.glsl");
	auto program = GraphicProgram::Create(*vertexShader, *fragmentShader);

	printf("%d\n", glIsProgram(program->program));

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

	while (!window->shouldClose()) {
		program->clear();
		program->start();

		vertexArray->render();

		program->finish();
		window->swap();
	}
	return 0;
}