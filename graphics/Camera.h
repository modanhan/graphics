#pragma once

#include "Geometry.h"

namespace RayTracing {
	class Camera
	{
	public:
		int width;
		int height;
		float fov;
		Camera(int width, int height, float fov)
			: width(width)
			, height(height)
			, fov(fov) {

		}
		~Camera() = default;

		Ray ray(int i, int j);
	};

}
