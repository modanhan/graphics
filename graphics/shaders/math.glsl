
#extension GL_ARB_shading_language_include : require

const float EPSILON = 0.0001;

float length2(vec3 v) {
	return dot(v, v);
}

vec3 proj(vec3 u, vec3 v) {
	return v * dot(u, v) / dot(v, v);
}

vec3 _random(vec3 v) {
	return fract(cross(cos(v * 273.1523) * 12.23, vec3(3.45, -12.67, 9.45))) * 2.0 - 1.0;
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
