
#version 430

struct camera_data {
	int width;
	int height;
	float focalLength;
};

struct sphere_data {
	vec3 position;
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

layout(location = 0) out vec4 FragmentDepth;
layout(location = 1) out vec4 FragmentNormal;

const float EPSILON = 0.0001;

struct hit {
	vec3 normal;
	float d;
};

hit createHit(vec3 normal, float d) {
	hit h;
	h.normal = normal;
	h.d = d;
	return h;
}

float length2(vec3 v) {
	return dot(v, v);
}

vec3 proj(vec3 u, vec3 v) {
	return v * dot(u, v) / dot(v, v);
}

vec3 _random(vec3 v) {
	return fract(cross(v * 5.31, vec3(23.45, 345.67, 89.45)) * 8.54) * 2 - 1;
}

vec2 quadratic(float a, float b, float c) {
	float d = b * b - 4 * a * c;
	if (d < 0) return vec2(-1e+38);
	float q = (b > 0) ?
		-0.5 * (b + sqrt(d)) :
		-0.5 * (b - sqrt(d));
	float x0 = q / a;
	float x1 = c / q;
	return vec2(min(x0, x1), max(x0, x1));
}

vec3 ray_direction (float i, float j) {
	vec3 origin = vec3(0);
	float x = i / camera.width;
	float y = j / camera.height;
	x = -1.0 + 2.0 * x;
	y = -1.0 + 2.0 * y;
	return normalize(vec3(x, y / camera.width * camera.height, -camera.focalLength));
}

hit sphere_hit(sphere_data s, vec3 origin, vec3 direction) {
	vec3 l = origin - s.position;
	float a = dot(direction, direction);
	float b = 2 * dot(direction, l);
	float c = dot(l, l) - s.radius * s.radius;
	vec2 q = quadratic(a, b, c);
	if (q.x < 0) {
		q.x = q.y;
		if (q.x < 0) return createHit(vec3(0), 1e+38);
	}
	return createHit(normalize(origin + direction * q.x - s.position), q.x);
}

hit triangle_hit(triangle_data tri, vec3 origin, vec3 direction) {
	vec3 edge1, edge2, h, s, q;
	float a, f, u, v;
	edge1 = tri.v1 - tri.v0;
	edge2 = tri.v2 - tri.v0;
	h = cross(direction, edge2);
	a = dot(edge1, h);
	f = 1.0 / a;
	s = origin - tri.v0;
	u = f * dot(s, h);
	if (u < 0.0 || u > 1.0)
		return createHit(vec3(0), 1e+38);
	q = cross(s, edge1);
	v = f * dot(direction, q);
	if (v < 0.0 || u + v > 1.0)
		return createHit(vec3(0), 1e+38);
	float t = f * dot(edge2, q);
	if (t > 0) {
		return createHit(normalize(cross(edge1, edge2)), t);
	}
	return createHit(vec3(0), 1e+38);
}

void main(void)
{
	float d = 1e+38;
	vec3 normal;
	vec3 camera_position = vec3(0);
	vec3 direction = ray_direction(gl_FragCoord.x, gl_FragCoord.y);
	for (int i = 0; i < spheres.length(); ++i) {
		hit h = sphere_hit(spheres[i], camera_position, direction);
		if (h.d < d) {
			d = h.d;
			normal = h.normal;
		}
	}
	for (int i = 0; i < triangles.length(); ++i) {
		hit h = triangle_hit(triangles[i], camera_position, direction);
		if (h.d < d) {
			d = h.d;
			normal = h.normal;
		}
	}

	if (d == 1e+38) {
		FragmentDepth.x = d;
		FragmentNormal = vec4(-direction, 1.0);
		return;
	}

	FragmentDepth.x = d;
	FragmentNormal = vec4(normal, 1.0);
}
