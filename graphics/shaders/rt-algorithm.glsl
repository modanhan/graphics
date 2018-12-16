
#extension GL_ARB_shading_language_include : require

vec3 ray_direction(vec2 fragCoord) {
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
	return createHit(normalize(origin + direction * q.x - s.position), q.x, s.emission);
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
	vec3 d_emission = vec3(0);
	for (int i = 0; i < spheres.length(); ++i) {
		hit h = sphere_hit(spheres[i], camera_position, direction);
		if (h.d < d) {
			d = h.d;
			d_emission = h.emission;
			normal = h.normal;
		}
	}
	for (int i = 0; i < triangles.length(); ++i) {
		hit h = triangle_hit(triangles[i], camera_position, direction);
		if (h.d < d) {
			d = h.d;
			d_emission = h.emission;
			normal = h.normal;
		}
	}

	if (d == 1e+38) {
		return d_emission;
	}

	vec3 hit_position = camera_position + direction * d;
	vec3 intensity = vec3(0);
	mat3 fragmentRandomRotate = mat3(rotationMatrix(_random(direction), 2.0));

	float fv = _random(gl_FragCoord.xyz).z;
	mat3 fragment_random_ray = mat3(1);
	for (int i = 0; i < ray_vec3s.length(); ++i) {
		vec3 r_emission = vec3(0);
		bool s_hit = false;

		vec3 n_d = fragment_random_ray * ray_vec3s[i];
		n_d = fragmentRandomRotate * n_d;
		if (dot(n_d, normal) < 0) n_d = -n_d;
		
		float r_d = 1e+38;
		for (int i = 0; i < spheres.length(); ++i) {
			hit h = sphere_hit(spheres[i], hit_position + n_d * EPSILON, n_d);
			if (h.d < r_d) {
				r_d = h.d;
				r_emission = h.emission;
			}
		}
		for (int i = 0; i < triangles.length(); ++i) {
			hit h = triangle_hit(triangles[i], hit_position + n_d * EPSILON, n_d);
			if (h.d < r_d) {
				r_d = h.d;
				r_emission = h.emission;
			}
		}
		if (r_d >= 1e+38) {
			r_emission = vec3(0);
		}
		intensity += r_emission;
	}
	return d_emission + (intensity / ray_vec3s.length());
}
