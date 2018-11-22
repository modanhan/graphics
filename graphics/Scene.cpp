/*
* Scene.cpp
*
*  Created on: Sep 10, 2018
*  Author: John Hall
*/

#include "Scene.h"

#include "RenderingEngine.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//**Must include glad and GLFW in this order or it breaks**
#include "glad/glad.h"
#include <GLFW/glfw3.h>

Scene::Scene(RenderingEngine* renderer) : renderer(renderer) {
	//Create a single triangle
	//Additional triangles can be created by pushing groups of three more vertices into the verts vector
	Geometry triangle;
	triangle.verts.push_back(glm::vec3(-0.6f, -0.4f, 1.0f));
	triangle.verts.push_back(glm::vec3(0.0f, 0.6f, 1.0f));
	triangle.verts.push_back(glm::vec3(0.6f, -0.4f, 1.0f));

	//Colors are stored per vertex in the order of the vertices
	triangle.colors.push_back(glm::vec3(1.0f, 0.0f, 0.0f));
	triangle.colors.push_back(glm::vec3(0.0f, 1.0f, 0.0f));
	triangle.colors.push_back(glm::vec3(0.0f, 0.0f, 1.0f));

	triangle.drawMode = GL_TRIANGLES;

	//Construct vao and vbos for the triangle
	RenderingEngine::assignBuffers(triangle);

	//Send the triangle data to the GPU
	//Must be done every time the triangle is modified in any way, ex. verts, colors, normals, uvs, etc.
	RenderingEngine::setBufferData(triangle);

	//Add the triangle to the scene objects
	objects.push_back(triangle);
}

Scene::~Scene() {

}

void Scene::displayScene() {
	renderer->RenderScene(objects);
}