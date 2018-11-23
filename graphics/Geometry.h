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
		// TODO: replace this with a transform instead
		glm::vec3 position;
		Geometry() = default;
		virtual ~Geometry() = default;
		virtual std::unique_ptr<RayCast> hit(const Ray& ray) {
			return nullptr;
		}
	};

	class SphereGeometry : public Geometry {
	public:
		float radius;
		std::unique_ptr<RayCast> hit(const Ray& ray) override;
	};

}

