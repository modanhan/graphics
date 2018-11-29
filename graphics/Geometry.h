#pragma once

#include <glm/glm.hpp>

#include <memory>

namespace RayTracing {
	struct Ray {
		glm::vec3 origin;
		glm::vec3 direction;
		Ray(glm::vec3 origin, glm::vec3 direction)
			: origin(origin)
			, direction(direction) {
		}
	};

	struct RayCast {
		float length;
	};

	struct SphereGeometry {
	public:
		glm::vec3 position;
		float p0;
		glm::vec3 emission;
		float radius;

		SphereGeometry(glm::vec3 position, float radius)
			: position(position)
			, radius(radius) {

		}
		~SphereGeometry() = default;
	};

	struct TriangleGeometry {
	public:
		glm::vec3 v0;
		float p0;
		glm::vec3 v1;
		float p1;
		glm::vec3 v2;
		float p2;

		TriangleGeometry(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
			: v0(v0)
			, v1(v1)
			, v2(v2) {

		}
		~TriangleGeometry() = default;
	};

}

