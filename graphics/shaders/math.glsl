
#extension GL_ARB_shading_language_include : require

const float EPSILON = 0.0001;

float length2(vec3 v) {
	return dot(v, v);
}

vec3 proj(vec3 u, vec3 v) {
	return v * dot(u, v) / dot(v, v);
}

vec3 _random(vec3 v) {
	return fract(cross(cos(v * 2793.1523) * 12.23, vec3(243.45, -172.67, 91776.45))) * 2.0 - 1.0;
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

// http://www.neilmendoza.com/glsl-rotation-about-an-arbitrary-axis/
mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}

mat3 rotationMatrixFromTo(vec3 from, vec3 to) {
	if (from == to) return mat3(1);
	if (from == -to) return mat3(-1);
	return mat3(rotationMatrix(cross(to, from), acos(dot(from, to))));
}
