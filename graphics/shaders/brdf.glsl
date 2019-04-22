#extension GL_ARB_shading_language_include : require

float roughness = 0.12;
float metallic = 0.18;
vec3 F0 = vec3(0.12);

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
