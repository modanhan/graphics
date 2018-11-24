#pragma once

#include "Geometry.h"

namespace RayTracing {

	class Camera
	{
	public:
		int width;
		int height;
		float focalLength;
		Camera(int width, int height, float focalLength)
			: width(width)
			, height(height)
			, focalLength(focalLength) {

		}
		~Camera() = default;

		Ray ray(int i, int j);
	};

}
