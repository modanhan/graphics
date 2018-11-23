#include "GraphicProgram.h"
#include "ShaderTools.h"

#include <iostream>

std::unique_ptr<Window> Window::Create(int width, int height) {
	auto window = std::unique_ptr<Window>(new Window());
	if (!glfwInit()) {
		std::cout << "ERROR: GLFW failed to initialize, TERMINATING" << std::endl;
		return nullptr;
	}

	//Attempt to create a window with an OpenGL 4.1 core profile context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	window->window = glfwCreateWindow(width, height, "Graphics", 0, 0);
	if (!window->window) {
		std::cout << "Program failed to create GLFW window, TERMINATING" << std::endl;
		glfwTerminate();
		return nullptr;
	}

	//Bring the new window to the foreground (not strictly necessary but convenient)
	glfwMakeContextCurrent(window->window);

	//Intialize GLAD (finds appropriate OpenGL configuration for your system)
	if (!gladLoadGL()) {
		std::cout << "GLAD init failed" << std::endl;
		return nullptr;
	}
	return window;
}

bool Window::shouldClose() {
	return glfwWindowShouldClose(window);
}

int Window::swap() {
	glfwSwapBuffers(window);
	glfwPollEvents();
	return 0;
}

std::unique_ptr<Shader> Shader::Create(GLenum type, std::string filename) {
	auto shader = std::unique_ptr<Shader>(new Shader());
	shader->shader = ShaderTools::CompileShader(type, ShaderTools::LoadSource(filename));
	return shader;
}

Shader::~Shader() {
	glDeleteShader(shader);
}

std::unique_ptr<GraphicProgram> GraphicProgram::Create(const Shader& vertex, const Shader& fragment) {
	auto graphicProgram = std::unique_ptr<GraphicProgram>(new GraphicProgram());
	graphicProgram->program = ShaderTools::LinkProgram(vertex.shader, fragment.shader);
	return graphicProgram;
}

GraphicProgram::~GraphicProgram() {
	glDeleteProgram(program);
}

int GraphicProgram::clear() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	return 0;
}

int GraphicProgram::start() {
	glUseProgram(program);
	return 0;
}

int GraphicProgram::finish() {
	glUseProgram(0);
	return 0;
}