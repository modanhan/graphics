#pragma once

#include <memory>
#include <string>
#include <vector>
#include <glad/glad.h>
#include <glfw/glfw3.h>

class Window {
	GLFWwindow* window = nullptr;
	Window() = default;
public:
	static std::unique_ptr<Window> Create(int width, int height);

	bool shouldClose();
	int swap();
};

class Shader {
	Shader() = default;
public:
	GLuint shader = 0;
	static std::unique_ptr<Shader> Create(GLenum type, std::string filename);
	static std::unique_ptr<Shader> Create(GLenum type, std::vector<std::string> files);
	~Shader();
};

class GraphicProgram {
	GraphicProgram() = default;
public:
	GLuint program = 0;
	static std::unique_ptr<GraphicProgram> Create(const Shader& vertex, const Shader& fragment);
	~GraphicProgram();

	int clear();
	int start();
	int finish();
};

