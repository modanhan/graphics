
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
		return createHit(normalize(cross(edge1, edge2)), t, tri.emission);
	}
	return createHit(vec3(0), 1e+38);
}

vec3 ray_trace0(vec3 position, vec3 direction) {
	float d = 1e+38;
	vec3 normal;
	vec3 d_emission = vec3(0);
	for (int i = 0; i < spheres.length(); ++i) {
		hit h = sphere_hit(spheres[i], position, direction);
		if (h.d < d) {
			d = h.d;
			d_emission = h.emission;
			normal = h.normal;
		}
	}
	for (int i = 0; i < triangles.length(); ++i) {
		hit h = triangle_hit(triangles[i], position, direction);
		if (h.d < d) {
			d = h.d;
			d_emission = h.emission;
			normal = h.normal;
		}
	}

	if (d == 1e+38) {
		return texture(ibl, SampleSphericalMap(direction)).xyz;
	}
	return d_emission;
}

vec3 ray_trace1(vec3 position, vec3 direction) {
	float d = 1e+38;
	vec3 normal;
	vec3 d_emission = vec3(0);
	for (int i = 0; i < spheres.length(); ++i) {
		hit h = sphere_hit(spheres[i], position, direction);
		if (h.d < d) {
			d = h.d;
			d_emission = h.emission;
			normal = h.normal;
		}
	}
	for (int i = 0; i < triangles.length(); ++i) {
		hit h = triangle_hit(triangles[i], position, direction);
		if (h.d < d) {
			d = h.d;
			d_emission = h.emission;
			normal = h.normal;
		}
	}

	if (d == 1e+38) {
		return texture(ibl, SampleSphericalMap(direction)).xyz;
	}

	vec3 hit_position = position + direction * d;
	vec3 intensity = vec3(0);

	mat3 normal_rotate = mat3(1);
	normal_rotate = rotationMatrixFromTo(vec3(0, 1, 0), normal);
	vec3 _rrv = _random(direction);
	mat3 random_rotate = mat3(rotationMatrix(normal, _rrv.x + _rrv.y + _rrv.z));

	int diffuse_count = int(32 * (1.0 - metallic)							); // downsample for secondary;
	for (int i = 0; i < diffuse_count; ++i) {
		vec3 n_d = vec3(ray_vec3s[i]);

		float is_angle = acos(n_d.y) / 3.1415926 * 0.5;
		mat3 is_rotate = mat3(rotationMatrix(cross(n_d, vec3(0, 1, 0)), -asin(is_angle) ));
		vec3 diffuse_d = random_rotate * normal_rotate * is_rotate * n_d;
		float diffuse_pdf = cos(-asin(is_angle));
		
		intensity += ray_trace0(hit_position + normal * EPSILON, diffuse_d)
			* dot(diffuse_d, normal)							* (1 - metallic) / diffuse_pdf; // diffuse
	}

	int specular_count = int(32 * pow(metallic, 1.0 / 3.0) * (1 - roughness)); // downsample for secondary;
	for (int i = 0; i < specular_count; ++i) {
		vec3 n_d = vec3(ray_vec3s[i]);

		// hacked importance sampling for specular, plz find source that remotely suggests this is correct
		vec3 specular_d = normalize(mix(random_rotate * normal_rotate * n_d, reflect(direction, normal), 1 - roughness));
		float specular_pdf = 1 / (roughness * roughness);
		
		intensity += ray_trace0(hit_position + normal * EPSILON, specular_d)
			* cook_torrance(-direction, normal, specular_d)		* (metallic) / specular_pdf; // specular
	}

	return d_emission + intensity / (diffuse_count + specular_count);
}

vec3 ray_trace2(vec3 position, vec3 direction) {
	float d = 1e+38;
	vec3 normal;
	vec3 d_emission = vec3(0);
	for (int i = 0; i < spheres.length(); ++i) {
		hit h = sphere_hit(spheres[i], position, direction);
		if (h.d < d) {
			d = h.d;
			d_emission = h.emission;
			normal = h.normal;
		}
	}
	for (int i = 0; i < triangles.length(); ++i) {
		hit h = triangle_hit(triangles[i], position, direction);
		if (h.d < d) {
			d = h.d;
			d_emission = h.emission;
			normal = h.normal;
		}
	}

	if (d == 1e+38) {
		return texture(ibl, SampleSphericalMap(direction)).xyz;
	}

	vec3 hit_position = position + direction * d;
	vec3 intensity = vec3(0);

	mat3 normal_rotate = mat3(1);
	normal_rotate = rotationMatrixFromTo(vec3(0, 1, 0), normal);
	vec3 _rrv = _random(direction);
	mat3 random_rotate = mat3(rotationMatrix(normal, _rrv.x + _rrv.y + _rrv.z));

	int diffuse_count = int(ray_vec3s.length() * (1.0 - metallic));
	for (int i = 0; i < diffuse_count; ++i) {
		vec3 n_d = vec3(ray_vec3s[i]);

		float is_angle = acos(n_d.y) / 3.1415926 * 0.5;
		mat3 is_rotate = mat3(rotationMatrix(cross(n_d, vec3(0, 1, 0)), -asin(is_angle) ));
		vec3 diffuse_d = random_rotate * normal_rotate * is_rotate * n_d;
		float diffuse_pdf = cos(-asin(is_angle));
		
		intensity += ray_trace1(hit_position + normal * EPSILON, diffuse_d)
			* dot(diffuse_d, normal)							* (1 - metallic) / diffuse_pdf; // diffuse
	}

	int specular_count = int(ray_vec3s.length() * pow(metallic, 1.0 / 3.0) * (1 - roughness));
	for (int i = 0; i < specular_count; ++i) {
		vec3 n_d = vec3(ray_vec3s[i]);

		// hacked importance sampling for specular, plz find source that remotely suggests this is correct
		vec3 specular_d = normalize(mix(random_rotate * normal_rotate * n_d, reflect(direction, normal), 1 - roughness));
		float specular_pdf = 1 / (roughness * roughness);
		
		intensity += ray_trace1(hit_position + normal * EPSILON, specular_d)
			* cook_torrance(-direction, normal, specular_d)		* (metallic) / specular_pdf; // specular
	}

	return d_emission + intensity / (diffuse_count + specular_count);
}
