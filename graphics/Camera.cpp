#include "Camera.h"

using namespace glm;

namespace RayTracing {

	Ray Camera::ray(int i, int j) {
		vec3 origin = vec3(0);
		float x = (i + 0.5) / width;
		float y = (j + 0.5) / height;
		x = -1.0 + 2.0 * x;
		y = -1.0 + 2.0 * y;
		return Ray(origin, vec3(x, y / width * height, -focalLength));
	}
}
