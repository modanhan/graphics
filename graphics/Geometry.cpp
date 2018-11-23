#include "Geometry.h"

#include <glm/gtx/norm.hpp>
#include <glm/gtx/projection.hpp>

using namespace glm;

namespace RayTracing {

	std::unique_ptr<RayCast> SphereGeometry::hit(const Ray& ray) {
		auto v_pc = position - ray.origin;
		if (dot(v_pc, ray.direction) < 0) {
			if (length2(v_pc) > radius * radius) {
				return nullptr;
			}
			auto pc = proj(position - ray.origin, ray.direction);
			auto d = sqrt(radius*radius - length2(pc - position));
			d -= length(ray.origin - pc);
			auto rayCast = std::make_unique<RayCast>();
			rayCast->length = d ;
			return std::move(rayCast);
		}
		else {
			auto pc = proj(position - ray.origin, ray.direction);
			if (length2(position - pc) > radius*radius) {
				return nullptr;
			}
			auto d = sqrt(radius*radius - length2(pc - position));
			if (length2(v_pc) > radius*radius) {
				d = length(pc - ray.origin) - d;
			}
			else {
				d = length(pc - ray.origin) + d;
			}
			auto rayCast = std::make_unique<RayCast>();
			rayCast->length = d;
			return std::move(rayCast);

		}
	}

}

