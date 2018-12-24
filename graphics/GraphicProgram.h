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

class Program {
	GLuint program = 0;
	Program() = default;
public:
	static std::unique_ptr<Program> CreateGraphic(const Shader& vertex, const Shader& fragment);
	static std::unique_ptr<Program> CreateCompute(const Shader& compute);
	~Program();

	int clear();
	int start();
	int finish();

	operator GLuint&() {
		return program;
	}
};

