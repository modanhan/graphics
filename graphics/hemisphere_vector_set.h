#pragma once
#include <vector>
#include <glm/glm.hpp>

extern std::vector<glm::vec3> hemisphere_vector_set;
std::vector<glm::vec3> hemisphere_halton(int size);