
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

float roughness = 0.15;
float metallic = 0.99;
vec3 F0 = vec3(0.98,  0.98, 0.98);

float d_ggx(vec3 v, vec3 n)
{
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
    float nm = dot(n, v);
    float denominator = (nm * nm * (alpha2 - 1) + 1);
    return alpha2 / (PI * denominator * denominator);
}

vec3 f_schlick(float cosT, vec3 F0)
{
	return F0 + (1 - F0) * pow(1 - cosT, 5);
}

float g_ggx(vec3 v, vec3 h) {
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha / 1;
	float hv = sdot(h, v);
	return 1 / (hv * (1 - alpha2) + alpha2);
}

float g_smith(vec3 i, vec3 o, vec3 h) {
	return g_ggx(i, h) * g_ggx(o, h);
}

vec3 cook_torrance(vec3 i, vec3 n, vec3 o) {
    vec3 h = normalize(o + i);
    return vec3(1)
		* d_ggx(n, h)
		* f_schlick(sdot(i, h), F0)
		* g_smith(i, o, n)
		* dot(n, o)
		;
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
		return texture(ibl, SampleSphericalMap(direction)).xyz;
	}

	vec3 hit_position = camera_position + direction * d;
	vec3 intensity = vec3(0);
	float intensity_max = 0;

	mat3 normal_rotate = mat3(1);
	normal_rotate = rotationMatrixFromTo(vec3(0, 1, 0), normal);
	vec3 _rrv = _random(direction);
	mat3 random_rotate = mat3(rotationMatrix(normal, _rrv.x + _rrv.y + _rrv.z));

	for (int i = 0; i < ray_vec3s.length(); ++i) {
		vec3 r_emission = vec3(0);
		bool s_hit = false;

		vec3 n_d = vec3(ray_vec3s[i]);

		// importance sampling
		float is_angle = acos(n_d.y) / 3.1415926 * 0.5;
		mat3 is_rotate = mat3(rotationMatrix(cross(n_d, vec3(0, 1, 0)), -asin(is_angle) ));
	//	n_d = is_rotate * n_d;
		n_d = normal_rotate * n_d;
		n_d = random_rotate * n_d;

	//	vec3 ref = reflect(direction, normal);
	//	n_d = normalize(mix(n_d, ref, 1 - roughness));

		float r_d = 1e+38;
		for (int i = 0; i < spheres.length(); ++i) {
			hit h = sphere_hit(spheres[i], hit_position + normal * EPSILON, n_d);
			if (h.d < r_d) {
				r_d = h.d;
				r_emission = h.emission;
			}
		}
		for (int i = 0; i < triangles.length(); ++i) {
			hit h = triangle_hit(triangles[i], hit_position + normal * EPSILON, n_d);
			if (h.d < r_d) {
				r_d = h.d;
				r_emission = h.emission;
			}
		}
		if (r_d >= 1e+38) {
			r_emission = texture(ibl, SampleSphericalMap(n_d)).xyz;
		}
		float pdf = cos(-asin(is_angle));
		pdf = 1;
	//	pdf = 1 / roughness / roughness;
		intensity += r_emission * dot(n_d, normal)							* (1 - metallic) / pdf; // diffuse
		intensity += r_emission * cook_torrance(-direction, normal, n_d)	* (metallic) / pdf; // specular
		intensity_max += 1;
	}
	return d_emission + (intensity / intensity_max);
}
