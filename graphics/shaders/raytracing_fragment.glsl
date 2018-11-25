
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

out vec4 FragmentColour;

const float EPSILON = 1e-6;

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

vec3 ray_direction (float i, float j) {
	vec3 origin = vec3(0);
	float x = i / camera.width;
	float y = j / camera.height;
	x = -1.0 + 2.0 * x;
	y = -1.0 + 2.0 * y;
	return normalize(vec3(x, y / camera.width * camera.height, -camera.focalLength));
}

hit sphere_hit(sphere_data s, vec3 origin, vec3 direction) {
	vec3 v_pc = s.position - origin;
	if (dot(v_pc, direction) < 0) {
		if (length2(v_pc) > s.radius * s.radius) {
			return createHit(vec3(0), 1e+38);
		}
		vec3 pc = proj(s.position - origin, direction);
		float d = sqrt(s.radius * s.radius - length2(pc - s.position));
		d -= length(origin - pc);
		return createHit(normalize(origin + direction * d - s.position), d);
	}
	else {
		vec3 pc = proj(s.position - origin, direction);
		if (length2(s.position - pc) > s.radius * s.radius) {
			return createHit(vec3(0), 1e+38);
		}
		float d = sqrt(s.radius * s.radius - length2(pc - s.position));
		if (length2(v_pc) > s.radius * s.radius) {
			d = length(pc - origin) - d;
		}
		else {
			d = length(pc - origin) + d;
		}
		return createHit(normalize(origin + direction * d - s.position), d);
	}
	return createHit(vec3(0), 1e+38);
}

hit triangle_hit(triangle_data tri, vec3 origin, vec3 direction) {
	vec3 edge1, edge2, h, s, q;
	float a, f, u, v;
	edge1 = tri.v1 - tri.v0;
	edge2 = tri.v2 - tri.v0;
	h = cross(direction, edge2);
	a = dot(edge1, h);
	if (a > -EPSILON && a < EPSILON)
		return createHit(vec3(0), 1e+38);    // This ray is parallel to this triangle.
	f = 1.0 / a;
	s = origin - tri.v0;
	u = f * dot(s, h);
	if (u < 0.0 || u > 1.0)
		return createHit(vec3(0), 1e+38);
	q = cross(s, edge1);
	v = f * dot(direction, q);
	if (v < 0.0 || u + v > 1.0)
		return createHit(vec3(0), 1e+38);
	// At this stage we can compute t to find out where the intersection point is on the line.
	float t = f * dot(edge2, q);
	if (t > EPSILON) // ray intersection
	{
		return createHit(normalize(cross(edge1, edge2)), t);
	}
	else // This means that there is a line intersection but not a ray intersection.
		return createHit(vec3(0), 1e+38);
}

void main(void)
{
	float d = 1e+38;
	vec3 color;
	for (int i = 0; i < spheres.length(); ++i) {
		hit h = sphere_hit(spheres[i], vec3(0), ray_direction(gl_FragCoord.x, gl_FragCoord.y));
		if (h.d < d) {
			d = h.d;
			color = h.normal;
		}
	}
	for (int i = 0; i < triangles.length(); ++i) {
		hit h = triangle_hit(triangles[i], vec3(0), ray_direction(gl_FragCoord.x, gl_FragCoord.y));
		if (h.d < d) {
			d = h.d;
			color = h.normal;
		}
	}

	if (d == 1e+38) {
		FragmentColour.xyz = vec3(0);
		return;
	}
	FragmentColour.xyz = color;
}
