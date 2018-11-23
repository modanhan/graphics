#include "Geometry.h"

#include <glm/gtx/norm.hpp>
#include <glm/gtx/projection.hpp>

using namespace glm;

namespace RayTracing {
	constexpr float EPSILON = 1e-6;

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

	// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
	std::unique_ptr<RayCast> TriangleGeometry::hit(const Ray& ray) {
		vec3 edge1, edge2, h, s, q;
		float a, f, u, v;
		edge1 = v1 - v0;
		edge2 = v2 - v0;
		h = cross(ray.direction, edge2);
		a = dot(edge1, h);
		if (a > -EPSILON && a < EPSILON)
			return nullptr;    // This ray is parallel to this triangle.
		f = 1.0 / a;
		s = ray.origin - v0;
		u = f * dot(s, h);
		if (u < 0.0 || u > 1.0)
			return nullptr;
		q = cross(s, edge1);
		v = f * dot(ray.direction, q);
		if (v < 0.0 || u + v > 1.0)
			return nullptr;
		// At this stage we can compute t to find out where the intersection point is on the line.
		float t = f * dot(edge2, q);
		if (t > EPSILON) // ray intersection
		{
			auto rayCast = std::make_unique<RayCast>();
			rayCast->length = t;
			return std::move(rayCast);
		}
		else // This means that there is a line intersection but not a ray intersection.
			return nullptr;
	}

}

