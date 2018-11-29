
#extension GL_ARB_shading_language_include : require

struct camera_data {
	int width;
	int height;
	float focalLength;
};

struct sphere_data {
	vec3 position;
	vec3 emission;
	float radius;
};

struct triangle_data {
	vec3 v0;
	vec3 v1;
	vec3 v2;
};

layout(std430, binding = 0) buffer camera_buffer { camera_data camera; };
layout(std430, binding = 1) buffer sphere_buffer { sphere_data[] spheres; };
layout(std430, binding = 2) buffer triangle_buffer { triangle_data[] triangles; };

struct hit {
	vec3 normal;
	float d;
	vec3 emission;
};

hit createHit(vec3 normal, float d) {
	hit h;
	h.normal = normal;
	h.d = d;
	return h;
}

hit createHit(vec3 normal, float d, vec3 emission) {
	hit h;
	h.normal = normal;
	h.d = d;
	h.emission = emission;
	return h;
}