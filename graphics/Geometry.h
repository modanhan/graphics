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

	class Geometry {
	public:
		Geometry() = default;
		virtual ~Geometry() = default;
		virtual std::unique_ptr<RayCast> hit(const Ray& ray) {
			return nullptr;
		}
	};

	class SphereGeometry : public Geometry {
	public:
		glm::vec3 position;
		float radius;
		std::unique_ptr<RayCast> hit(const Ray& ray) override;

		SphereGeometry(glm::vec3 position, float radius)
			: position(position)
			, radius(radius) {

		}
		~SphereGeometry() = default;
	};

	class TriangleGeometry : public Geometry {
	public:
		glm::vec3 v0;
		glm::vec3 v1;
		glm::vec3 v2;

		std::unique_ptr<RayCast> hit(const Ray& ray) override;

		TriangleGeometry(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2)
			: v0(v0)
			, v1(v1)
			, v2(v2) {

		}
		~TriangleGeometry() = default;
	};

}

