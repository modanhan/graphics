
#extension GL_ARB_shading_language_include : require

vec3 ray_direction (vec2 fragCoord) {
	vec3 origin = vec3(0);
	vec2 d = fragCoord / vec2(camera.width, camera.height) * 2.0 - 1.0;
	return normalize(vec3(d.x, d.y / camera.width * camera.height, -camera.focalLength));
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

vec3 ray_trace(vec3 direction) {
	float d = 1e+38;
	vec3 normal;
	vec3 camera_position = vec3(0);
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
		return vec3(1);
	}

	vec3 hit_position = camera_position + direction * d;
	float intensity = 0;
	vec3 n_d = hit_position;
	int iterations = 512;
	for (int i = 0; i < iterations; ++i) {
		bool s_hit = false;

		n_d = normalize(_random(n_d));
		if (dot(n_d, normal) < 0) n_d = -n_d;
		if (n_d.y < 0) continue;

		for (int i = 0; i < spheres.length(); ++i) {
			hit h = sphere_hit(spheres[i], hit_position + n_d * EPSILON, n_d);
			if (h.d < 1e+38) {
				s_hit = true;
				break;
			}
		}
		if (!s_hit)
		for (int i = 0; i < triangles.length(); ++i) {
			hit h = triangle_hit(triangles[i], hit_position + n_d * EPSILON, n_d);
			if (h.d < 1e+38) {
				s_hit = true;
				break;
			}
		}
		if (!s_hit)
			intensity += 1.0;
	}
	return vec3(intensity / iterations);
}